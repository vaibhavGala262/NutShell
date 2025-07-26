#ifndef PARSERS_H
#define PARSERS_H

#define MAX_ARGS 128
#define MAX_COMMANDS 64
#define MAX_REDIRECTIONS 8

typedef enum {
   
    REDIRECT_NONE,
    REDIRECT_OUTPUT,        // >
    REDIRECT_APPEND,        // >>
    REDIRECT_INPUT,         // <
    REDIRECT_ERROR,         // 2>
    REDIRECT_BOTH ,         // &>
   
} redirect_type_t;  // for Redirection i/o


typedef struct {
    redirect_type_t type;
    char *filename;
    int fd;  // File descriptor number (for 2>, 3>, etc.)
} redirection_t;

typedef enum {
    EXEC_NORMAL,
    EXEC_AND,     // Execute only if previous succeeded (&&)
    EXEC_OR       // Execute only if previous failed (||)
} exec_condition_t;

// Add this enum after your existing enums
typedef enum {
    OP_NONE,        // No operator
    OP_SEMICOLON,   // ;
    OP_AND,         // &&
    OP_OR,          // ||
    OP_PIPE,        // |
    OP_BACKGROUND   // &
} operator_type_t;


typedef struct {
    char *args[MAX_ARGS];
    int is_background;
    redirection_t redirections[MAX_REDIRECTIONS];
    int redirect_count;
    exec_condition_t condition; 
    operator_type_t next_operator;  
} command_t;


typedef struct {
    command_t commands[MAX_COMMANDS];
    int count;
    int is_pipeline;
} command_list_t;

void parse_command_line(char *input, command_list_t *cmd_list);
void free_command_list(command_list_t *cmd_list);

#endif
