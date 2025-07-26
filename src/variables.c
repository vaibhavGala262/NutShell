#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "variables.h"
#include <ctype.h>
#include "debug.h"


static var_table_t var_table;

// Simple hash function
static unsigned int hash_function(const char *str) {
    unsigned int hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return hash % HASH_TABLE_SIZE;
}

void init_variables(void) {
    // Initialize hash table
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        var_table.buckets[i] = NULL;
    }
    
    // Import common environment variables
    char *env_vars[] = {"PATH", "HOME", "USER", "PWD", "SHELL", NULL};
    for (int i = 0; env_vars[i]; i++) {
        char *value = getenv(env_vars[i]);
        if (value) {
            set_variable(env_vars[i], value, 1);  // 1 = exported
        }
    }
    
    // Set current working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd))) {
        set_variable("PWD", cwd, 1);
    }
}

int set_variable(const char *name, const char *value, int export_flag) {
    if (!name || !value) return -1;
    
    unsigned int hash = hash_function(name);
    variable_t *var = var_table.buckets[hash];
    
    // Check if variable already exists
    while (var) {
        if (strcmp(var->name, name) == 0) {
            // Update existing variable
            free(var->value);
            var->value = strdup(value);
            var->is_exported = export_flag;
            
            // Update environment if exported
            if (export_flag) {
                setenv(name, value, 1);
            }
            return 0;
        }
        var = var->next;
    }
    
    // Create new variable
    variable_t *new_var = malloc(sizeof(variable_t));
    if (!new_var) return -1;
    
    new_var->name = strdup(name);
    new_var->value = strdup(value);
    new_var->is_exported = export_flag;
    new_var->next = var_table.buckets[hash];
    var_table.buckets[hash] = new_var;
    
    // Add to environment if exported
    if (export_flag) {
        setenv(name, value, 1);
    }
    
    return 0;
}

char* get_variable(const char *name) {
    if (!name) return NULL;
    
    unsigned int hash = hash_function(name);
    variable_t *var = var_table.buckets[hash];
    
    while (var) {
        if (strcmp(var->name, name) == 0) {
            return var->value;
        }
        var = var->next;
    }
    
    // If not found in our table, check environment
    return getenv(name);
}

int unset_variable(const char *name) {
    if (!name) return -1;
    
    unsigned int hash = hash_function(name);
    variable_t *var = var_table.buckets[hash];
    variable_t *prev = NULL;
    
    while (var) {
        if (strcmp(var->name, name) == 0) {
            // Remove from hash table
            if (prev) {
                prev->next = var->next;
            } else {
                var_table.buckets[hash] = var->next;
            }
            
            // Remove from environment if exported
            if (var->is_exported) {
                unsetenv(name);
            }
            
            free(var->name);
            free(var->value);
            free(var);
            return 0;
        }
        prev = var;
        var = var->next;
    }
    
    return -1;  // Variable not found
}

void list_variables(void) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        variable_t *var = var_table.buckets[i];
        while (var) {
            DEBUG_VERBOSE("%s=%s", var->name, var->value);
            if (var->is_exported) {
                DEBUG_VERBOSE(" (exported)");
            }
            DEBUG_VERBOSE("\n");
            var = var->next;
        }
    }
}

// Variable expansion function
char* expand_variables(const char *input) {
    if (!input) return NULL;
    
    DEBUG_VERBOSE("Expanding: '%s'\n", input);
    
    char *result = malloc(strlen(input) * 4 + 1);
    char *result_ptr = result;
    const char *input_ptr = input;
    
    while (*input_ptr) {
        DEBUG_VERBOSE("Processing char: '%c'\n", *input_ptr);  // NEW DEBUG
        
        if (*input_ptr == '@') {
            DEBUG_INFO("Found @ symbol\n");  // NEW DEBUG
            input_ptr++;  // Skip $
            
            if (*input_ptr == '{') {
                // Handle ${VAR} syntax - your existing code
            } else {
                // Handle $VAR syntax
                char var_name[MAX_VAR_NAME];
                int i = 0;
                
                DEBUG_INFO("Collecting variable name starting with: '%c'\n", *input_ptr);  // NEW DEBUG
                
                while (*input_ptr && (isalnum(*input_ptr) || *input_ptr == '_') && 
                       i < MAX_VAR_NAME - 1) {
                    var_name[i++] = *input_ptr++;
                    DEBUG_VERBOSE("Added char to var_name: '%c', var_name so far: '%.*s'\n", 
                           *(input_ptr-1), i, var_name);  // NEW DEBUG
                }
                var_name[i] = '\0';
                
                DEBUG_VERBOSE("Final variable name: '%s'\n", var_name);  // NEW DEBUG
                
                if (i > 0) {
                    char *var_value = get_variable(var_name);
                    DEBUG_VERBOSE("Variable $%s = '%s'\n", var_name, var_value ? var_value : "(null)");
                    if (var_value) {
                        strcpy(result_ptr, var_value);
                        result_ptr += strlen(var_value);
                    }
                } else {
                    DEBUG_WARN("No variable name found, keeping $\n");  // NEW DEBUG
                    *result_ptr++ = '$';
                }
            }
        } else {
            *result_ptr++ = *input_ptr++;
        }
    }
    
    *result_ptr = '\0';
    DEBUG_INFO ("Expanded result: '%s'\n", result);
    return result;
}

void cleanup_variables(void) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        variable_t *var = var_table.buckets[i];
        while (var) {
            variable_t *next = var->next;
            free(var->name);
            free(var->value);
            free(var);
            var = next;
        }
        var_table.buckets[i] = NULL;
    }
}
