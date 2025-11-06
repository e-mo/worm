#include <errno.h>

#include <git2.h>

#include "worm_error.h"
#include "worm_cfg.h"
#include "prompt.h"


enum worm_clone_state {
    CLONE_SUCCESS = 0,
    CLONE_SKIP,
    CLONE_SSH_AUTH_REQUEST,
    CLONE_BAD_KEY_PATH,
    CLONE_PASSPHRASE_REQUEST,
    CLONE_DIR_EXISTS
};

static void repo_cleanup(git_repository **repo);

static int _clone_cfg(worm_cfg *config);
static int _clone_repository(worm_cfg *config);

static constexpr int MAX_AUTH_ATTEMPTS = 3;

int worm_clone(const char *root) {
    int status = -1;
    worm_cfg_stack stack = {0};
            
    // Parse first worm file.
    status = worm_cfg_file_parse(root, &stack);
    if (status != WORM_OK)
        goto RETURN;

    int skips = 0;
    while (worm_cfg_stack_is_empty(&stack) == false) {
        worm_cfg config = worm_cfg_stack_pop(&stack);

        // Move cwd to the module root.
        if (chdir(config.root) == -1)
            REPORT_ABORT(ERR_FD, "SYSTEM", "chdir failed > %s", strerror(errno));

        // We only want to print special ANSI formatting when outputting to a terminal.
        bool is_tty = isatty(fileno(stdout));

        if (is_tty) printf(ANSI_UNDERLINE);
        printf("\nCLONING\n");
        if (is_tty) printf(ANSI_RESET);
        printf("├╴REMOTE: %s\n", config.remote);
        printf("└─╴LOCAL: %s/%s\n", config.root, config.local);

        switch(_clone_cfg(&config)) {
        case CLONE_SUCCESS:
            status = worm_cfg_file_parse(config.local, &stack);
            
            // Found and parsed another worm config file.
            if (status == WORM_OK) {
                printf("Nested '%s/worm.toml' parsed.", config.local);
                continue;
            }
            // Nothing to parse. 
            if (status == WORM_ERROR_CFG_NOT_FOUND)
                continue;

            // File found, but failed to parse. 
            if (status == WORM_ERROR_CFG_PARSE) {
                printf("Nested '%s/worm.toml' found, but parsing failed.", config.local);
                break;
            }

            // If we reach here, we didn't handle something. Print bug and abort. 
            REPORT_ABORT(ERR_FD, "BUG", "unhandled return status -> %s", status);
            __builtin_unreachable();

        // SSH key was not found.
        case CLONE_BAD_KEY_PATH:
            printf("Bad path to SSH key -> '%s'\n", config.ssh.private);
            break;

        // Bad passphrase after max attempts.
        case CLONE_PASSPHRASE_REQUEST:
            printf("Max passprhase attempts reached.\n");
            break;

        case CLONE_SKIP:
            printf("Skipping '%s'...\n", config.remote);
            break;
            
        default:
            REPORT_ABORT(ERR_FD, "BUG", "unhandled return value > %d", status);
        }

        skips += 1;
    }

RETURN:
    status = skips ? WORM_ERROR : WORM_OK;
    return status;
}

static void repo_cleanup(git_repository **repo) {
    if (*repo != nullptr)
        git_repository_free(*repo);
}

static int _clone_cfg(worm_cfg *config) {
    int status = -1;

    int auth_attempts = 0;
    for (;;) {
        switch (status = _clone_repository(config)) {
        case CLONE_SUCCESS:
            [[fallthrough]];
        case CLONE_BAD_KEY_PATH:
            goto RETURN;

        // TODO: Add prompt asking if user wishes to clobber folder.
        case CLONE_DIR_EXISTS:
            printf("Warning: local directory '%s' already exists.\n", config->local);
            printf("         Cloning will CLOBBER existing contents.\n");

            // 'yes' or 'no' explicit check to clobber.
            // Anything other than an explicit 'yes' will skip clonging and preserve existing
            // contents of local directory.
            char choice[256]; 
            prompt_user(choice, 256, true,
                        "Would you like to proceed and clobber '%s'? (yes/no): ", config->local);

            if (!strcmp(choice, "yes")) {
                printf("Clobbering '%s'...\n", config->local);
                char command[PATH_MAX + 7];
                sprintf(command, "rm -rf %s", config->local);
                system(command);
            } else {
                printf("Not authorized to clobber '%s'.\n", config->local);
                status = CLONE_SKIP;
                goto RETURN;
            }

            continue;

        case CLONE_SSH_AUTH_REQUEST:
            printf("SSH authentication requested...\n");
            if (!strlen(config->ssh.private))
                prompt_user(config->ssh.private, PATH_MAX, true, "Path to private key file: ");

            config->gco.fetch_opts.callbacks.credentials = worm_cfg_ssh_auth_cb;
            config->gco.fetch_opts.callbacks.payload = config;

            continue;

        case CLONE_PASSPHRASE_REQUEST:
            if (auth_attempts == MAX_AUTH_ATTEMPTS)
                goto RETURN;

            auth_attempts += 1;

            prompt_user(config->ssh.passphrase, SSH_KEY_PASSPHRASE_MAX, false,
                    "Passphrase required for encrypted key file '%s': ", config->ssh.private);

            fflush(stdout);

            continue;
        }
    }

RETURN:
    return status;
}

static int _clone_repository(worm_cfg *config) {
    git_repository *repo CLEANUP(repo_cleanup) = nullptr;

    int git_err = git_clone(&repo, config->remote, config->local, &config->gco);
    
    const git_error *last = git_error_last();
    switch (last->klass) {

    case GIT_ERROR_NONE:
        if (git_err == GIT_OK)
            return CLONE_SUCCESS;
        break;

    case GIT_ERROR_SSH:
        // SSH auth requested.
        if (git_err == GIT_EAUTH)
           return CLONE_SSH_AUTH_REQUEST;

        // Path to key file is invalid.
        if (strstr(last->message, "Unable to open") != nullptr)        
            return CLONE_BAD_KEY_PATH;

        // Passphrase incorrect or not set.
        if (strstr(last->message, "Wrong passphrase") != nullptr)
            return CLONE_PASSPHRASE_REQUEST;
        break;

    case GIT_ERROR_INDEX:
        if (git_err == GIT_OK)
            return CLONE_SUCCESS;
        break;

    case GIT_ERROR_INVALID:
        // Directory already exists.
        if (git_err == GIT_EEXISTS)
            return CLONE_DIR_EXISTS;
        break;
    }
    REPORT_ABORT(ERR_FD, "BUG", "Unexpected libgit2 error > (%d/%d: %s).", git_err, last->klass, last->message);
    __builtin_unreachable();
}
