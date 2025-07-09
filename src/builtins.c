#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "builtins.h"

int handle_builtin(char **args) {
    if (args[0] == NULL) return 0;

    if (strcmp(args[0], "exit") == 0) exit(0);

    if (strcmp(args[0], "cdir") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: missing argument\n");
        } else if (chdir(args[1]) != 0) {
            perror("cd failed");
        }
        return 1;
    }

    if (strcmp(args[0], "clearscreen") == 0) {
        system("clear");
        return 1;
    }

    if (strcmp(args[0], "pcd") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd))) {
            printf("%s\n", cwd);
        } else {
            perror("pwd failed");
        }
        return 1;
    }

    return 0;
}
