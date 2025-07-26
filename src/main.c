#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <unistd.h> // get cur dir (getcwd)
#include <getopt.h>

#include "debug.h"
#include "builtins.h"
#include "parsers.h"
#include "executor.h"
#include "history.h"
#include <bits/waitflags.h>
#include <sys/wait.h>
#include <sched.h>
#include <utils.h>
#include <variables.h>

#define MAX_CMD_LEN 1024

void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("Options:\n");
    printf("  -d, --debug LEVEL    Set debug level (0-4)\n");
    printf("                       0=NONE, 1=ERROR, 2=WARN, 3=INFO, 4=VERBOSE\n");
    printf("  -v, --verbose        Enable verbose debug output (same as -d 4)\n");
    printf("  -q, --quiet          Disable all debug output (same as -d 0)\n");
    printf("  -h, --help           Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s                   # Run normally (no debug)\n", program_name);
    printf("  %s -v                # Run with verbose debug\n", program_name);
    printf("  %s --debug 2         # Run with warnings and errors\n", program_name);
}

// Check if a command is a variable assignment
int is_assignment(const char *str)
{
    if (!str)
        return 0;

    // Looking for VAR=VALUE pattern
    char *equals = strchr(str, '=');
    if (!equals || equals == str)
        return 0; // No = or starts with = (Should have = atleast)

    // Check that everything before = is a valid variable name
    for (char *p = (char *)str; p < equals; p++)
    {
        if (!isalnum(*p) && *p != '_')
            return 0;
    }

    return 1;
}

void process_assignment(const char *assignment)
{
    char *copy = strdup(assignment);
    char *equals = strchr(copy, '=');

    *equals = '\0';
    char *name = copy;
    char *value = equals + 1;

    // Remove quotes from value (handles both single and double quotes)
    char *clean_value = value;
    int len = strlen(value);

    if (len >= 2 &&
        ((value[0] == '"' && value[len - 1] == '"') ||
         (value[0] == '\'' && value[len - 1] == '\'')))
    {
        // Remove both opening and closing quotes
        value[len - 1] = '\0';   // Remove closing quote
        clean_value = value + 1; // Skip opening quote
    }

    // Expand variables in the clean value
    char *expanded_value = expand_variables(clean_value);
    set_variable(name, expanded_value, 0);

    DEBUG_INFO("Set variable %s = '%s'\n", name, expanded_value); 

    free(expanded_value);
    free(copy);
}

void cleanup_and_exit(int signo)
{
    printf("\n[Shell] Saving history and exiting...\n");

    // Save history to file
    save_history_to_file();

    // Cleanup resources
    cleanup_history();

    printf("Goodbye!\n");
    exit(0);
}

void sigchld_handler(int signo)
{
    DEBUG_INFO("SIGCHLD handler called\n");
    fflush(stdout);

    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        DEBUG_VERBOSE("SIGCHLD: Reaped child PID %d\n", pid);
        fflush(stdout);
        printf("\n[Job completed] PID: %d\n", pid);
        printf("%s", get_fancy_prompt());
        fflush(stdout);
    }

    DEBUG_INFO("SIGCHLD handler finished\n");
    fflush(stdout);
}

int main(int argc, char *argv[])
{   
    int opt;
    struct option long_options[] = {
        {"debug",   required_argument, 0, 'd'},
        {"verbose", no_argument,       0, 'v'},
        {"quiet",   no_argument,       0, 'q'},
        {"help",    no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };
    
    while ((opt = getopt_long(argc, argv, "d:vqh", long_options, NULL)) != -1) {
        switch (opt) {
            case 'd': {
                int level = atoi(optarg);
                if (level >= 0 && level <= 4) {
                    set_debug_level((debug_level_t)level);
                } else {
                    fprintf(stderr, "Invalid debug level: %d (must be 0-4)\n", level);
                    return 1;
                }
                break;
            }
            case 'v':
                set_debug_level(DEBUG_VERBOSE);
                break;
            case 'q':
                set_debug_level(DEBUG_NONE);
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    // Show debug level if enabled to user
    if (g_debug_level > DEBUG_NONE) {
        DEBUG_INFO("Debug level set to: %s", debug_level_to_string(g_debug_level));
    }
    
    char *input;
    init_variables();
    init_history(); // initialize history system

    // initializing readline lib not needed

    signal(SIGCHLD, sigchld_handler);
    signal(SIGINT, cleanup_and_exit);  // Ctrl+C
    signal(SIGTERM, cleanup_and_exit); // Termination
    signal(SIGQUIT, cleanup_and_exit); // Quit signal

    atexit(cleanup_history); // Registering  cleanup function for normal exit

    while (1)
    {
        // readline handles history automatically with up/down arrows
        input = readline(get_colored_prompt());
        DEBUG_VERBOSE("Input received: '%s' (length: %zu)\n", input ? input : "NULL", input ? strlen(input) : 0);

        // Check for EOF (Ctrl+D)  
        if (input == NULL)
        {
            printf("\n[Shell] EOF detected, saving history...\n");
            save_history_to_file();
            break;
        }

        // Skip empty lines
        if (strlen(input) == 0)
        {
            free(input);
            continue;
        }

        add_history(input);    // readline's built-in history function
        add_to_history(input); // my custom history function

        if (is_assignment(input))
        {
            process_assignment(input);
            free(input);
            continue;
        }

        command_list_t cmd_list;
        parse_command_line(input, &cmd_list);

        // Execute commands, grouping them into pipelines
     
       // Execute commands with logical operator support
int last_exit_status = 0;  // Track the exit status of the last command

for (int i = 0; i < cmd_list.count; i++) {
    command_t *cmd = &cmd_list.commands[i];
    
    // Skip empty commands
    if (cmd->args[0] == NULL) {
        continue;
    }
    
    // Determine if we should execute this command based on previous result
    int should_execute = 1;
    
    if (i > 0) {
        // Check the operator from the previous command
        operator_type_t prev_operator = cmd_list.commands[i-1].next_operator;
        
        if (prev_operator == OP_AND) {
            // && : Execute only if previous command succeeded (exit status 0)
            should_execute = (last_exit_status == 0);
            DEBUG_INFO("&& operator: last_status=%d, executing=%s", 
                      last_exit_status, should_execute ? "yes" : "no");
        } else if (prev_operator == OP_OR) {
            // || : Execute only if previous command failed (exit status != 0)
            should_execute = (last_exit_status != 0);
            DEBUG_INFO("|| operator: last_status=%d, executing=%s", 
                      last_exit_status, should_execute ? "yes" : "no");
        }
        // For OP_SEMICOLON, OP_PIPE, OP_NONE - always execute (should_execute stays 1)
    }
    
    if (should_execute) {
        DEBUG_INFO("Executing: %s", cmd->args[0]);
        
        // Check if it's a built-in command with your existing logic
        if (cmd->redirect_count > 0) {
            // Execute with redirections
            execute_command_with_redirections(cmd);
            last_exit_status = 0;  // Assume success for redirected commands
        } else if (handle_builtin(cmd->args)) {
            // Built-in command executed
            last_exit_status = 0;  // Built-ins assume success
        } else {
            // Execute external command and capture exit status
            last_exit_status = execute_external_command_with_status(cmd->args, cmd->is_background);
        }
        
        DEBUG_INFO("Command '%s' exited with status: %d", cmd->args[0], last_exit_status);
    } else {
        DEBUG_INFO("Skipping: %s (condition not met)", cmd->args[0]);
    }
}

free_command_list(&cmd_list);

//  Free the input allocated by readline
free(input);
}
cleanup_variables();
return 0;
}