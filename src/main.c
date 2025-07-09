#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtins.h"
#include "parsers.h"
#include "executor.h"
#include "history.h"


#define MAX_CMD_LEN 1024






int main() {
    char input[MAX_CMD_LEN];

     
    while (1) {
        printf("nutshell> ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }
        
        input[strcspn(input, "\n")] = 0;
        if (strlen(input) > 0) {
            add_to_history(input);
}
        
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
    }
    
    return 0;
}