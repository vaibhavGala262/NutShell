#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
int main() {
    int x = 10;
    pid_t pid = fork();

    if (pid == 0) {
        x += 5;
        printf("👶 Child: x = %d, PID = %d\n", x, getpid());
    } else {
        x -= 5;
        printf("👨 Parent: x = %d, PID = %d\n", x, getpid());
    }

    return 0;
}
