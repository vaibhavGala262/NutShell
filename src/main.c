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

        // Add to both readline history and your custom history
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
     
        int i = 0;
        while (i < cmd_list.count)
        {
            // Find pipeline 
            int pipeline_start = i;
            int pipeline_end = i;
            int is_background = 0;

           
            while (pipeline_end < cmd_list.count - 1)
            {
                // Check if current command is followed by a pipe
              
                break;
            }

            // Check if the last command in pipeline is background
            if (pipeline_end < cmd_list.count && cmd_list.commands[pipeline_end].is_background)
            {
                is_background = 1;
            }

            int pipeline_length = pipeline_end - pipeline_start + 1;

            DEBUG_INFO("Command list count: %d\n", cmd_list.count);
            DEBUG_INFO("Pipeline start: %d, end: %d, length: %d\n", pipeline_start, pipeline_end, pipeline_length);

            if (pipeline_start < cmd_list.count && cmd_list.commands[pipeline_start].args[0])
            {
                DEBUG_VERBOSE("Command 0: '%s'\n", cmd_list.commands[pipeline_start].args[0]);
            }
            else
            {
                DEBUG_ERROR("Command 0: NULL or invalid\n");
            }

            // Skip empty commands
            if (pipeline_start >= cmd_list.count || cmd_list.commands[pipeline_start].args[0] == NULL)
            {
                i = pipeline_end + 1;
                continue;
            }

            DEBUG_INFO("About to execute commands\n");
            DEBUG_INFO("Checking if '%s' is built-in\n", cmd_list.commands[pipeline_start].args[0]);

            // Check if it's a built-in command
            if (pipeline_length == 1)
            {
                // Check if command has redirections
                if (cmd_list.commands[pipeline_start].redirect_count > 0)
                {
                    // Execute with redirections (works for both built-ins and external commands , note normal execution did not work for both)
                    execute_command_with_redirections(&cmd_list.commands[pipeline_start]);
                }
                else
                {
                    // No redirections so , check if built-in
                    if (handle_builtin(cmd_list.commands[pipeline_start].args))
                    {
                        // Built-in handled without redir
                    }
                    else
                    {
                        // External command without redir
                        execute_command(cmd_list.commands[pipeline_start].args, is_background);
                    }
                }
            }
        else
    {
        DEBUG_INFO("Executing pipeline with %d commands\n", pipeline_length);
        // Pipeline with potential redirections
        execute_pipeline(&cmd_list.commands[pipeline_start], pipeline_length, is_background);
    }

    i = pipeline_end + 1;
}

free_command_list(&cmd_list);

//  Free the input allocated by readline
free(input);
}
cleanup_variables();
return 0;
}