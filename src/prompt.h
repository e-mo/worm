#ifndef WORM_PROMPT_H
#define WORM_PROMPT_H

#include <termios.h>
#include <string.h>
#include <stdarg.h>

static inline char * prompt_user(char *buf, ptrdiff_t buf_size, bool echo, const char *prompt, ...) {
    va_list args;
    va_start(args, prompt);
    vprintf(prompt, args);
    va_end(args);

    fflush(stdout);
    
    // Disable echo.
    struct termios old_term = {0};
    if (!echo) {
        tcgetattr(STDIN_FILENO, &old_term);

        struct termios new_term = old_term;
        new_term.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
    }

    // Read input.
    if (fgets(buf, buf_size, stdin) == NULL)
        buf[0] = '\0';
    // Remove newline.
    else {
        char *newline = strchr(buf, '\n');
        if (newline != nullptr)
            *newline = '\0';
    }
        
    
    // Restore echo.
    if (!echo) {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
        printf("\n");
    }
    
    return buf;
}

#endif // WORM_PROMPT_H
