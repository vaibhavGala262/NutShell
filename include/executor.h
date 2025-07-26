#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parsers.h"

void execute_command(char **args, int is_background);
void execute_command_with_redirections(command_t *cmd); // handle redirection commands
void execute_pipeline(command_t *commands, int cmd_count, int is_background);  // handle pipeline execution
int execute_external_command_with_status(char **args, int is_background);




#endif
