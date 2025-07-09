#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtins.h"
#include "parsers.h"
#include "executor.h"

#define MAX_CMD_LEN 1024

int main() {
    char input[MAX_CMD_LEN];
    char *args[MAX_ARGS];

    while (1) {
        printf("nutshell> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = 0;

        parse_input(input, args);

        if (handle_builtin(args)) continue;

        execute_command(args);
    }

    return 0;
}
