#include <stdio.h>
#include <stddef.h>
#include <assert.h>
// MAX_PATH
#include <limits.h>
// Terminal Control.
#include <termios.h>
#include <unistd.h>
// libgit2
#include <git2.h>

// Internal headers.
#include "ansi.h"       // ANSI escape codes (for terminal text formatting).
#include "worm_error.h" // Error handling.
#include "worm_clone.h" // Clone command.

#include "args.h"

int cmd_help(void) {
    args_print_usage();
    return 0;
}

int cmd_clone(const char *config_root) {
    printf("Attempting to parse 'worm.toml' from root '%s'.\n", config_root);
    
    int status = worm_clone(config_root);
    switch(status) {
    case WORM_OK:
        printf("Worm clone compeleted.\n");
        break;
    case WORM_ERROR:
        printf("Worm clone compeleted with errors.\n");
        printf("Warning: Some repositories may not have been cloned properly.\n");
        break;
    case WORM_ERROR_CFG_BAD_ROOT: 
        printf("Worm config not found: bad root path.\n");
        break;
    case WORM_ERROR_CFG_NOT_FOUND: 
        printf("Worm config not found: no 'worm.toml' file at root path.\n");
        break;
    case WORM_ERROR_CFG_PARSE:
        printf("Worm config bad format: unable to parse 'worm.toml'.\n");
        break;
    default:
        REPORT_ERROR(ERR_FD, "BUG", "unhandled return value > %d", status);
    }
    
    return status;
}


int main(int argc, char **argv) {
    args_t args = args_parse(argc, argv);

    if (args.command == CMD_NONE)
        return WORM_ERROR;

    int git_err;
    if ((git_err = git_libgit2_init()) < 0) {
        print_git_error(git_err);
        return WORM_ERROR_GIT;
    }

    int status = -1;
    switch (args.command) {
        case CMD_HELP:
            status = cmd_help();
            break;

        case CMD_CLONE:
            status = cmd_clone(args.config_root);
            break;
            
        case CMD_NONE:
            status = WORM_ERROR;;  // Shouldn't reach here
            break;
    }

    while ((git_err = git_libgit2_shutdown()) > 0);
    if (git_err != GIT_OK)
        print_git_error(git_err);

    return status;
}
