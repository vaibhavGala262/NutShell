#include <string.h>
#include "parsers.h"

void parse_input(char *input, char **args) {
    int i = 0;
    char *saveptr;
    char *token = strtok_r(input, " ", &saveptr);
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok_r(NULL, " ", &saveptr);
    }
    args[i] = NULL;
}