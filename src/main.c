#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "builtins.h"
#include "parsers.h"
#include "executor.h"
#include "history.h"

#define MAX_CMD_LEN 1024

int main() {
    char *input;
    
    // Initialize readline - enable tab completion
    rl_bind_key('\t', rl_complete);
    
    while (1) {
        // readline handles history automatically with up/down arrows
        input = readline("nutshell> ");
        
        // Check for EOF (Ctrl+D)
        if (input == NULL) {
            printf("\n");
            break;
        }
        
        // Skip empty lines
        if (strlen(input) == 0) {
            free(input);
            continue;
        }
        
        // Add to both readline history and your custom history
        add_history(input);        // readline's built-in history
        add_to_history(input);     // your custom history function
        
        // Use strtok_r instead of strtok to avoid interference with parse_input
        char *saveptr_main;
        char *cmd = strtok_r(input, ";", &saveptr_main);
        
        while (cmd != NULL) {
            // Remove leading/trailing spaces (optional polish)
            while (*cmd == ' ') cmd++;
            
            char *args[MAX_ARGS];
            
            // Parse individual command
            char cmd_copy[MAX_CMD_LEN];
            strncpy(cmd_copy, cmd, MAX_CMD_LEN);
            cmd_copy[MAX_CMD_LEN - 1] = '\0';  // Ensure null termination
            
            printf("[Shell] Next command: %s\n", cmd);
            
            parse_input(cmd_copy, args);
            
            if (args[0] == NULL) {
                cmd = strtok_r(NULL, ";", &saveptr_main);
                continue;
            }
            
            // Built-in or external command
            if (handle_builtin(args)) {
                // handled by built-in
            } else {
                execute_command(args);  // fork + exec
            }
            
            cmd = strtok_r(NULL, ";", &saveptr_main);
        }
        
        // IMPORTANT: Free the input allocated by readline
        free(input);
    }
    
    return 0;
}