#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "executor.h"

void execute_command(char **args) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
    } else if (pid == 0) {
        execvp(args[0], args);
        perror("Command failed");
        exit(1);
    } else {
        wait(NULL);
    }
}
