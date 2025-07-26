#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>  // For open() flags
#include <errno.h>
#include "executor.h"
#include <signal.h>
#include <debug.h>

// Helper functions  // Note dont define static fun in .h files
static int has_output_redirection(command_t *cmd) {
    for (int i = 0; i < cmd->redirect_count; i++) {
        if (cmd->redirections[i].type == REDIRECT_OUTPUT ||
            cmd->redirections[i].type == REDIRECT_APPEND ||
            cmd->redirections[i].type == REDIRECT_BOTH) {
            return 1;
        }
    }
    return 0;
}

static int has_input_redirection(command_t *cmd) {
    for (int i = 0; i < cmd->redirect_count; i++) {
        if (cmd->redirections[i].type == REDIRECT_INPUT) {
            return 1;
        }
    }
    return 0;
}


// Apply redirections for a command
static int apply_redirections(command_t *cmd) {
    for (int i = 0; i < cmd->redirect_count; i++) {
        redirection_t *redir = &cmd->redirections[i];
        int fd;
        
        switch (redir->type) {
            case REDIRECT_OUTPUT:
                // Open file for writing (create/truncate)
                fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) {
                    perror("Failed to open output file");
                    return -1;
                }
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    perror("Failed to redirect stdout");
                    close(fd);
                    return -1;
                }
                close(fd);
                break;
                
            case REDIRECT_APPEND:
                // Open file for appending
                fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (fd == -1) {
                    perror("Failed to open append file");
                    return -1;
                }
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    perror("Failed to redirect stdout for append");
                    close(fd);
                    return -1;
                }
                close(fd);
                break;
                
            case REDIRECT_INPUT:
                // Open file for reading
                fd = open(redir->filename, O_RDONLY);
                if (fd == -1) {
                    perror("Failed to open input file");
                    return -1;
                }
                if (dup2(fd, STDIN_FILENO) == -1) {
                    perror("Failed to redirect stdin");
                    close(fd);
                    return -1;
                }
                close(fd);
                break;
                
            case REDIRECT_ERROR:
                // Open file for error output
                fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) {
                    perror("Failed to open error file");
                    return -1;
                }
                if (dup2(fd, STDERR_FILENO) == -1) {
                    perror("Failed to redirect stderr");
                    close(fd);
                    return -1;
                }
                close(fd);
                break;
                
            case REDIRECT_BOTH:
                // Redirect both stdout and stderr to same file
                fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) {
                    perror("Failed to open combined output file");
                    return -1;
                }
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    perror("Failed to redirect stdout");
                    close(fd);
                    return -1;
                }
                if (dup2(fd, STDERR_FILENO) == -1) {
                    perror("Failed to redirect stderr");
                    close(fd);
                    return -1;
                }
                close(fd);
                break;
                
            default:
                break;
        }
    }
    
    return 0;
}

void execute_command(char **args, int is_background) {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        return;
    }
    
    if (pid == 0) {
        // Child process
        
        // Reset signals to default
        for (int i = 1; i < 32; i++) {
            signal(i, SIG_DFL);
        }
        
        execvp(args[0], args);
        perror("execvp failed");
        _exit(1);
    } else {
        // Parent process
        if (!is_background) {
            int status;
            waitpid(pid, &status, 0);
        } else {
            printf("[Background job] PID: %d\n", pid);
        }
    }
}

// New function to execute command with redir
void execute_command_with_redirections(command_t *cmd) {
    // Checking if its a built-in command
    int is_builtin = 0;
    
    // List of built-in commands
    char *builtins[] = {"env", "set", "export", "unset", "cdir", "pcd", "history", "exit", NULL};
    for (int i = 0; builtins[i]; i++) {
        if (strcmp(cmd->args[0], builtins[i]) == 0) {
            is_builtin = 1;
            break;
        }
    }
    
    if (is_builtin) {
        // Handle built-in with redirections in current process
        
        // Save original file descriptors  //<---Very Imp for builtin to enable redirections---> 
        int saved_stdout = dup(STDOUT_FILENO);
        int saved_stderr = dup(STDERR_FILENO);
        int saved_stdin = dup(STDIN_FILENO);
        
        // Apply redirections
        if (apply_redirections(cmd) == -1) {
            // Restore original FDs on error
            dup2(saved_stdout, STDOUT_FILENO);
            dup2(saved_stderr, STDERR_FILENO);
            dup2(saved_stdin, STDIN_FILENO);
            close(saved_stdout);
            close(saved_stderr);
            close(saved_stdin);
            return;
        }
        
        // Execute the built-in command
        handle_builtin(cmd->args);
        
        // Restore original file descriptors
        dup2(saved_stdout, STDOUT_FILENO);
        dup2(saved_stderr, STDERR_FILENO);
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdout);
        close(saved_stderr);
        close(saved_stdin);
        
    } else {
        // Handle external command with redir
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("fork failed");
            return;
        }
        
        if (pid == 0) {
            // Child process
            for (int i = 1; i < 32; i++) {
                signal(i, SIG_DFL);
            }
            
            if (apply_redirections(cmd) == -1) {
                _exit(1);
            }
            
            execvp(cmd->args[0], cmd->args);
            perror("execvp failed");
            _exit(1);
        } else {
            // Parent process
            if (!cmd->is_background) {
                int status;
                waitpid(pid, &status, 0);
            } else {
                printf("[Background job] PID: %d\n", pid);
            }
        }
    }
}

// Enhanced pipeline execution with redirection support
void execute_pipeline(command_t *commands, int cmd_count, int is_background) {
    if (cmd_count == 1) {
        // Single command - check for redirections
        if (commands[0].redirect_count > 0) {
            execute_command_with_redirections(&commands[0]);
        } else {
            execute_command(commands[0].args, is_background);
        }
        return;
    }
    
    int pipes[cmd_count - 1][2];
    pid_t pids[cmd_count];
    
    // Create all pipes
    for (int i = 0; i < cmd_count - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed");
            return;
        }
    }
    
    // Fork and execute each command
    for (int i = 0; i < cmd_count; i++) {
        pids[i] = fork();
        
        if (pids[i] == -1) {
            perror("fork failed");
            return;
        }
        
        if (pids[i] == 0) {
            // Child process
            
            // Set up pipes (only if no conflicting redirections)
            if (i > 0 && !has_input_redirection(&commands[i])) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            
            if (i < cmd_count - 1 && !has_output_redirection(&commands[i])) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            
            // Close all pipe file descriptors
            for (int j = 0; j < cmd_count - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            // Apply any redirections for this command
            if (apply_redirections(&commands[i]) == -1) {
                _exit(1);
            }
            
            execvp(commands[i].args[0], commands[i].args);
            perror("execvp failed");
            _exit(1);
        }
    }
    
    // Parent: close all pipes
    for (int i = 0; i < cmd_count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    // Wait for all processes
    if (!is_background) {
        for (int i = 0; i < cmd_count; i++) {
            int status;
            waitpid(pids[i], &status, 0);
        }
    } else {
        DEBUG_INFO("[Background pipeline] PIDs: ");
        for (int i = 0; i < cmd_count; i++) {
            DEBUG_VERBOSE("%d ", pids[i]);
        }
        DEBUG_INFO("\n");
    }
}


// Get exit status from a command execution
int get_command_exit_status(char **args, int is_background) {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        return 1;  // Return failure status
    }
    
    if (pid == 0) {
        // Child process
        for (int i = 1; i < 32; i++) {
            signal(i, SIG_DFL);
        }
        
        execvp(args[0], args);
        perror("execvp failed");
        _exit(127);  // Command not found
    } else {
        // Parent process
        if (!is_background) {
            int status;
            waitpid(pid, &status, 0);
            
            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);  // Return actual exit code
            } else {
                return 1;  // Command terminated abnormally
            }
        } else {
            printf("[Background job] PID: %d\n", pid);
            return 0;  // Background jobs always "succeed" for chaining
        }
    }
}

