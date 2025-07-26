#ifndef VARIABLES_H
#define VARIABLES_H

#define MAX_VAR_NAME 256
#define MAX_VAR_VALUE 1024
#define HASH_TABLE_SIZE 128

typedef struct variable {
    char *name;
    char *value;
    int is_exported;  // Whether it's an environment variable
    struct variable *next;  // For hash table collision chaining
} variable_t;

typedef struct {
    variable_t *buckets[HASH_TABLE_SIZE];
} var_table_t;

// Variable management functions
void init_variables(void);
void cleanup_variables(void);
int set_variable(const char *name, const char *value, int export_flag);
char* get_variable(const char *name);
int unset_variable(const char *name);
void list_variables(void);

// Variable expansion
char* expand_variables(const char *input);



#endif
