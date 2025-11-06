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
#include "ansi.h"   // ANSI escape codes.
#include "worm_clone.h"
#include "worm_error.h"

#include "layer/macros.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    int git_err;
    if ((git_err = git_libgit2_init()) < 0) {
        print_git_error(git_err);
        return WORM_ERROR_GIT;
    }

    // TODO: accept an arg and use it here.
    int status = worm_clone(nullptr);
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

    while ((git_err = git_libgit2_shutdown()) > 0);
    if (git_err != GIT_OK)
        print_git_error(git_err);

    return status;
}
