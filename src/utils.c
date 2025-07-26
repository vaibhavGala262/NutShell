#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include "utils.h"

// Simple current directory prompt
char* get_simple_prompt() {
    static char prompt[1024];
    char cwd[512];
    
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        snprintf(prompt, sizeof(prompt), "nutshell:%s> ", cwd);
    } else {
        strcpy(prompt, "nutshell> ");
    }
    
    return prompt;
}

// Bash-style prompt with home directory shortening
char* get_fancy_prompt() {
    static char prompt[1024];
    char cwd[512];
    char *home_dir = getenv("HOME");
    
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        char display_path[512];
        
        // Replace home directory with ~
        if (home_dir && strncmp(cwd, home_dir, strlen(home_dir)) == 0) {
            if (strlen(cwd) == strlen(home_dir)) {
                strcpy(display_path, "~");
            } else {
                snprintf(display_path, sizeof(display_path), "~%s", cwd + strlen(home_dir));
            }
        } else {
            strcpy(display_path, cwd);
        }
        
        snprintf(prompt, sizeof(prompt), "nutshell:%s> ", display_path);
    } else {
        strcpy(prompt, "nutshell> ");
    }
    
    return prompt;
}

// Full bash-style with username and hostname
char* get_full_prompt() {
    static char prompt[1024];
    char cwd[512];
    char hostname[256];
    char *username = getenv("USER");
    char *home_dir = getenv("HOME");
    
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        strcpy(cwd, "unknown");
    }
    
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        strcpy(hostname, "localhost");
    }
    
    // Shorten home directory to ~
    char display_path[512];
    if (home_dir && strncmp(cwd, home_dir, strlen(home_dir)) == 0) {
        if (strlen(cwd) == strlen(home_dir)) {
            strcpy(display_path, "~");
        } else {
            snprintf(display_path, sizeof(display_path), "~%s", cwd + strlen(home_dir));
        }
    } else {
        strcpy(display_path, cwd);
    }
    
    snprintf(prompt, sizeof(prompt), "%s@%s:%s$ ", 
             username ? username : "user", hostname, display_path);
    
    return prompt;
}

// Colored prompt 
char* get_colored_prompt() {
    static char prompt[1024];
    char cwd[512];
    char *home_dir = getenv("HOME");
    
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        char display_path[512];
        
        if (home_dir && strncmp(cwd, home_dir, strlen(home_dir)) == 0) {
            if (strlen(cwd) == strlen(home_dir)) {
                strcpy(display_path, "~");
            } else {
                snprintf(display_path, sizeof(display_path), "~%s", cwd + strlen(home_dir));
            }
        } else {
            strcpy(display_path, cwd);
        }
        
        // Green nutshell, blue path, reset color
        snprintf(prompt, sizeof(prompt), 
                "\033[32mnutshell\033[0m:\033[34m%s\033[0m> ", display_path);
    } else {
        strcpy(prompt, "\033[32mnutshell\033[0m> ");
    }
    
    return prompt;
}
