#include <stdio.h>
#include <string.h>

#include "args.h"

constexpr char cmd_name[] = "worm"; 

void args_print_usage(void) {
    printf("Usage: %s <command> [options]\n\n", cmd_name);
    printf("Commands:\n");
    printf("  help              Show this help message\n");
    printf("  clone [path]      Clone repositories from config file 'path/worm.toml'\n");
    printf("                    (default: ./worm.toml)\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s help\n", cmd_name);
    printf("  %s clone\n", cmd_name);
    printf("  %s clone /path/to/worm.toml\n", cmd_name);
} 

args_t args_parse(int argc, char *argv[]) {
    args_t args = {
        .command = CMD_NONE,
        .config_root = nullptr,
    };
    
    // Need at least program name + command
    if (argc < 2) {
        fprintf(stderr, "Error: No command specified\n\n");
        args_print_usage();
        return args;
    }
    
    const char *cmd = argv[1];
    
    // Parse command
    if (strcmp(cmd, "help") == 0) {
        args.command = CMD_HELP;
        // Ignore any extra arguments
        
    } else if (strcmp(cmd, "clone") == 0) {
        args.command = CMD_CLONE;
        
        // Optional config path argument
        if (argc >= 3) {
            args.config_root = argv[2];
        } else {
            args.config_root = ".";  // Default
        }
        
        // Warn if too many arguments
        if (argc > 3) {
            fprintf(stderr, "Warning: Extra arguments ignored\n");
        }
        
    } else {
        fprintf(stderr, "Error: Unknown command '%s'\n\n", cmd);
        args_print_usage();
        return args;
    }
    
    return args;
}
