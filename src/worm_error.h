#ifndef WORM_ERROR_H
#define WORM_ERROR_H

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include "ansi.h"

#include "layer/macros.h"

enum worm_errors {
   WORM_OK = 0,
   WORM_ERROR, // Non-specific error for contextual use.

   // Config file related errors.
   WORM_ERROR_CFG_BAD_ROOT, 
   WORM_ERROR_CFG_NOT_FOUND, 
   WORM_ERROR_CFG_PARSE,
   WORM_ERROR_CFG_LOCAL_BAD_PATH,

   // Git function errors.
   WORM_ERROR_GIT,
};

#define REPORT_ERROR(FD, TAG, FMT, ...) __report_error(FD, TAG, __FILE__, __LINE__, __func__, FMT __VA_OPT__(,) __VA_ARGS__)
#define REPORT_ABORT(FD, TAG, FMT, ...) \
do { \
    __report_error(FD, TAG, __FILE__, __LINE__, __func__, FMT __VA_OPT__(,) __VA_ARGS__); \
    abort(); \
} while (0)

static inline void __report_error(FILE *fd, const char *tag, const char *file, int line, const char *func, const char *fmt, ...) {
    // We only want to print special ANSI formatting when outputting to a terminal.
    bool is_tty = isatty(fileno(fd));

    if (is_tty) fprintf(fd, ANSI_UNDERLINE);
    fprintf(fd, "%s\n", tag);
    if (is_tty) fprintf(fd, ANSI_RESET);
    
    fprintf(fd, "├╴FILE: %s\n", file);
    fprintf(fd, "├╴LINE: %d\n", line);
    fprintf(fd, "└╴FUNC: %s\n", func);
    fprintf(fd, "> ");

    va_list args;
    va_start(args, fmt);
    vfprintf(fd, fmt, args);
    va_end(args);

    fprintf(fd, "\n");
}




static inline void print_git_error(int error) {
    const git_error *e = git_error_last();
    fprintf(ERR_FD, "libgit2: error\n\t%d/%d\n\t%s\n", error, e->klass, e->message);
}

#endif // WORM_ERROR_H
