#ifndef _LAYER_ARGS_H
#define _LAYER_ARGS_H

typedef enum {
    CMD_NONE,
    CMD_HELP,
    CMD_CLONE,
} command_t;

typedef struct {
    command_t command;
    const char *config_root;  // For clone command
} args_t;

// Parse command line arguments
// Returns parsed args, or sets command to CMD_NONE on error
args_t args_parse(int argc, char *argv[]);

// Print usage information
void args_print_usage(void);


#endif // _LAYER_ARGS_H
