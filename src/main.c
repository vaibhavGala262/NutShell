#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>  
#define MAX_CMD_LEN 1024 
#define MAX_ARGS 64



int main(){
    char input[MAX_CMD_LEN];  // store user command
    char *args[MAX_ARGS];


    while(1){
        printf("nutshell>");
        fflush(stdout); // ensure prompt is printed immediately


        if(fgets(input , sizeof(input) , stdin)==NULL){
            printf("\n");   // break shell loop on ctr+D
            break;   
        }
         input[strcspn(input, "\n")] = 0; // remove trailing newline character


         if (strcmp(input, "exit") == 0) {
            // if user types exit, break the loop
            break;
        }

        
        // split input into arguments
        int i=0;
        char * token  = strtok(input, " ");

        while(token!=NULL && i<MAX_ARGS-1){
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i]=NULL; // null-terminate the args array


        if (strcmp(args[0], "cdir") == 0) {
    if (args[1] == NULL) {
        fprintf(stderr, "cd: missing argument\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("cd failed");
        }
    }
    continue;  // skip fork
    }

    if (strcmp(args[0], "clearscreen") == 0) {
    system("clear");
    continue;
}


   
     if (strcmp(args[0], "pcd") == 0) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("pwd failed");
    }
    continue;
}






        pid_t pid = fork();

        
        if (pid < 0) {
            perror("fork failed");
        } else if (pid == 0) {
            // Inside child → run the command
            execvp(args[0], args);

            // If we’re still here, execvp failed
            perror("Command failed");
            exit(1);
        } else {
            // Parent waits
            
            wait(NULL);
        }

    }
        return 0;

    }




    

