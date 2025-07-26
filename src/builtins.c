#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "builtins.h"
#include "history.h"
#include "variables.h"
#include "debug.h"

int handle_builtin(char **args)
{
   
    if (args[0] == NULL)
        return 0;

    if (strcmp(args[0], "exit") == 0)
        exit(0);

    if (strcmp(args[0], "cdir") == 0)
    {
        if (args[1] == NULL)
        {
            DEBUG_WARN( "cd: missing argument\n");
        }
        else if (chdir(args[1]) != 0)
        {
            DEBUG_ERROR("cd failed: %s", strerror(errno));

        }
        return 1;
    }

    if (strcmp(args[0], "history") == 0)
    {
        if (args[1] && strcmp(args[1], "-c") == 0)
        {
            // history -c to clear history
            cleanup_history();
            init_history();
            printf("History cleared\n");
        }
        else if (args[1] && strcmp(args[1], "-w") == 0)
        {
            // Write/save history immediately
            save_history_to_file();
        }
        else
        {
            // Show history
            print_history();
        }
        return 1;
    }

    if (strcmp(args[0], "clearscreen") == 0)
    {
        system("clear");
        return 1;
    }

    if (strcmp(args[0], "pcd") == 0)
    {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)))
        {
            printf("%s\n", cwd);
        }
        else
        {
            DEBUG_ERROR("pcd failed: %s", strerror(errno));
        }
        return 1;
    }

    if (strcmp(args[0], "export") == 0)
    {
        if (args[1] == NULL)
        {
            // List all exported variables
            list_variables();
        }
        else
        {
            // Parse VAR=VALUE or just VAR
            char *equals = strchr(args[1], '=');
            if (equals)
            {
                *equals = '\0'; // Split name and value
                char *name = args[1];
                char *value = equals + 1;

                // Expand variables in the value
                char *expanded_value = expand_variables(value);
                set_variable(name, expanded_value, 1); // 1 = export
                free(expanded_value);

                printf("Exported %s=%s\n", name, get_variable(name));
            }
            else
            {
                // Export existing variable
                char *value = get_variable(args[1]);
                if (value)
                {
                    set_variable(args[1], value, 1);
                    printf("Exported %s\n", args[1]);
                }
                else
                {
                    DEBUG_ERROR("Variable %s not found\n", args[1]);
                }
            }
        }
        return 1;
    }

    if (strcmp(args[0], "set") == 0)
    {
        if (args[1] == NULL)
        {
            list_variables();
        }
        else
        {
            char *equals = strchr(args[1], '=');
            if (equals)
            {
                *equals = '\0';
                char *name = args[1];
                char *value = equals + 1;

                char *expanded_value = expand_variables(value);
                set_variable(name, expanded_value, 0); // 0 = not exported
                free(expanded_value);

                DEBUG_INFO("Set %s=%s\n", name, get_variable(name));
            }
            else
            {
                DEBUG_INFO("Usage: set VAR=VALUE\n");
            }
        }
        return 1;
    }

    if (strcmp(args[0], "unset") == 0)
    {
        if (args[1] == NULL)
        {
            printf("Usage: unset VAR\n");
        }
        else
        {
            if (unset_variable(args[1]) == 0)
            {
                printf("Unset %s\n", args[1]);
            }
            else
            {
                DEBUG_WARN("Variable %s not found\n", args[1]);
            }
        }
        return 1;
    }

    if (strcmp(args[0], "env") == 0) {
    DEBUG_VERBOSE(" env command called\n");
    
    extern char **environ;
    DEBUG_VERBOSE("environ pointer: %p\n", (void*)environ);
    
    if (environ == NULL) {
        DEBUG_ERROR("environ is NULL!\n");
        return 1;
    }
    
    if (environ[0] == NULL) {
        DEBUG_ERROR("environ[0] is NULL - empty environment!\n");
        return 1;
    }
    
    DEBUG_VERBOSE(" About to print environment...\n");
    
    int count = 0;
    for (int i = 0; environ[i]; i++) {
        printf("%s\n", environ[i]);
        count++;
        
        if (count > 200) {  // Safety break
            DEBUG_WARN(" Stopped at 200 vars for safety\n");
            break;
        }
    }
    
    DEBUG_VERBOSE("Printed %d environment variables\n", count);
    DEBUG_VERBOSE("env command completed\n");
    return 1;
}


    return 0;
}
