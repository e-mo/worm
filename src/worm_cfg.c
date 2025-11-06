#include <stdio.h>
#include <string.h>

#include "toml-c.h"

#include "worm_cfg.h"
#include "worm_error.h"

#include "layer/macros.h"


static void file_cleanup(FILE **fp);
static void toml_cleanup(toml_table_t **tbl);

int worm_cfg_ssh_auth_cb(git_credential **cred, const char *url, const char *username, unsigned int at, void *payload) {
    (void)url;
    (void)at;

    const worm_cfg* config = (worm_cfg *)payload;

    const char *key_priv = nullptr;
    if (strlen(config->ssh.private))
        key_priv = config->ssh.private;
    
    const char *key_pub = nullptr;
    if (strlen(config->ssh.public))
        key_pub = config->ssh.public;

    const char *key_pass = nullptr;
    if (strlen(config->ssh.passphrase))
        key_pass = config->ssh.passphrase;

    return git_credential_ssh_key_new(cred, username, key_pub, key_priv, key_pass);
}

bool worm_cfg_stack_push(worm_cfg_stack *restrict stack, worm_cfg *restrict config) {
    if (stack == nullptr || config == nullptr)
        REPORT_ABORT(ERR_FD, "BUG", "unexpected null arguments.");

    if (stack->index == WORM_CFG_STACK_MAX)
        return false;

    stack->buf[stack->index] = *config;
    stack->index += 1;

    return true;
}

worm_cfg worm_cfg_stack_pop(worm_cfg_stack *stack) {
    if (stack == nullptr)
        REPORT_ABORT(ERR_FD, "BUG", "unexpected null argument.");

    stack->index -= 1;
    return stack->buf[stack->index];
}

bool worm_cfg_stack_is_empty(worm_cfg_stack *stack) {
    return stack->index == 0; 
}

static void file_cleanup(FILE **fp) {
    if (*fp != nullptr) {
        fclose(*fp);
        *fp = nullptr;
    }
}

static void toml_cleanup(toml_table_t **tbl) {
    if (*tbl != nullptr) {
        toml_free(*tbl);
        *tbl = nullptr;
    }
}

int worm_cfg_file_parse(const char *root, worm_cfg_stack *stack) {
    int status = WORM_OK;

    // Auto cleanup
    FILE *worm_fd CLEANUP(file_cleanup) = nullptr;
    toml_table_t *tbl CLEANUP(toml_cleanup) = nullptr;

    // Buffer cwd. It will be used as root, or need to be returned to.
    char cwd[PATH_MAX];
    if (getcwd(cwd, PATH_MAX) == nullptr)
        REPORT_ABORT(ERR_FD, "SYSTEM", "getcwd failed > %s", strerror(errno));

    // If passed a root directory, try to change to that directory..
    char root_abs[PATH_MAX] = {0};
    if (root != nullptr) {
        if (chdir(root) == -1) {
            status = WORM_ERROR_CFG_BAD_ROOT;
            goto RETURN;
        }

        // Make sure we have the absolute root path.
        if (getcwd(root_abs, PATH_MAX) == nullptr)
            REPORT_ABORT(ERR_FD, "SYSTEM", "getcwd failed > %s", strerror(errno));
    }

    // If root moved, use new root. Otherwise, use cwd.
    root = root_abs[0] ? root_abs : cwd; 

    if ((worm_fd = fopen(WORM_CFG_FILENAME, "r")) == nullptr) { 
        status = WORM_ERROR_CFG_NOT_FOUND;
        goto RETURN;
    }

    char toml_err[200];
    if ((tbl = toml_parse_file(worm_fd, toml_err, sizeof(toml_err))) == nullptr) {
        fprintf(stderr, "toml-c: error > %s\n", toml_err);
        status = WORM_ERROR_CFG_PARSE; 
        goto RETURN;
    }
        
    toml_array_t *submodule_array = toml_table_array(tbl, "submodule");
    if (!submodule_array) {
        status = WORM_ERROR_CFG_PARSE;
        goto RETURN;
    }

    for (int i = 0; i < toml_array_len(submodule_array); i++) {

        toml_table_t *module = toml_array_table(submodule_array, i);
        if (!module) {
            status = WORM_ERROR_CFG_PARSE;
            goto RETURN;
        }

        worm_cfg config = {0};
        strcpy(config.root, root);

        // toml_value_t name = toml_table_string(module, "name");
        // if (!name.ok) {
        //     status = WORM_ERROR_CFG_PARSE;
        //     goto RETURN;
        // }
        // strcpy(config.name, name.u.s);
                
        toml_value_t remote = toml_table_string(module, "remote");
        if (!remote.ok) {
            status =  WORM_ERROR_CFG_PARSE;
            goto RETURN;
        }
        strcpy(config.remote, remote.u.s);

        toml_value_t local = toml_table_string(module, "local");
        if (!local.ok) {
            status = WORM_ERROR_CFG_PARSE;
            goto RETURN;
        }
        strcpy(config.local, local.u.s);

        toml_value_t ssh_priv = toml_table_string(module, "ssh");
        toml_value_t ssh_pub = toml_table_string(module, "ssh_public");

        if (ssh_priv.ok)
            strcpy(config.ssh.private, ssh_priv.u.s);

        if (ssh_pub.ok)
            strcpy(config.ssh.public, ssh_pub.u.s);

        git_clone_options_init(&config.gco, GIT_CLONE_OPTIONS_VERSION);

        worm_cfg_stack_push(stack, &config);
    }

RETURN:
    // if (tbl != nullptr)
    //     toml_free(tbl);

    // If the root was moved, move back.
    if (root_abs[0] && chdir(cwd) == -1)
        REPORT_ABORT(ERR_FD, "SYSTEM", "chdir failed > %s", strerror(errno));
    
    return status;
}
