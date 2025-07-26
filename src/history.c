#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "history.h"
#include <debug.h>

static char *history[MAX_HISTORY];
static int history_count = 0;
static int history_modified = 0;  // Track if we need to save

// Get history file path
static char* get_history_file_path(void) {
    static char path[1024];
    char *home = getenv("HOME");
    
    if (home) {
        snprintf(path, sizeof(path), "%s/%s", home, HISTORY_FILE);
    } else {
        strcpy(path, HISTORY_FILE);  // Fallback to current directory
    }
    
    return path;
}

void init_history(void) {
    // Initialize history array
    for (int i = 0; i < MAX_HISTORY; i++) {
        history[i] = NULL;
    }
    
    // Load existing history from file
    load_history_from_file();
}

void add_to_history(const char *cmd) {
    if (!cmd || strlen(cmd) == 0) return;
    
    // Don't add duplicate consecutive commands
    if (history_count > 0 && history[history_count - 1] && 
        strcmp(history[history_count - 1], cmd) == 0) {
        return;
    }
    
    // If history is full, remove oldest entry (circular buffer behavior by bash mimiced  )
    if (history_count >= MAX_HISTORY) {
        free(history[0]);
        // Shift all entries left
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            history[i] = history[i + 1];
        }
        history_count = MAX_HISTORY - 1;
    }
    
    // Add new command
    history[history_count] = strdup(cmd);
    history_count++;
    history_modified = 1;
}

void print_history(void) {
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i + 1, history[i]);
    }
}

void load_history_from_file(void) {
    FILE *file = fopen(get_history_file_path(), "r");
    if (!file) return;  // File doesn't exist yet, that's OK
    
    char line[1024];
    while (fgets(line, sizeof(line), file) && history_count < MAX_HISTORY) {

        // Remove newline
        line[strcspn(line, "\n")] = 0;
        
        if (strlen(line) > 0) {
            history[history_count] = strdup(line);
            history_count++;
        }
    }
    
    fclose(file);
    printf("Loaded %d commands from history\n", history_count);
}

void save_history_to_file(void) {
    if (!history_modified) {
        return;  // No changes to save 
    }
    
    FILE *file = fopen(get_history_file_path(), "w");
    if (!file) {
        perror("Failed to save history");
        return;
    }
    
    // Writing all commands to file
    for (int i = 0; i < history_count; i++) {
        if (history[i]) {
            DEBUG_VERBOSE("%s\n", history[i]);
        }
    }
    
    fclose(file);
    history_modified = 0;  // Reset modification flag
    printf("History saved to %s\n", get_history_file_path());
}

void cleanup_history(void) {
    // Save before cleanup
    save_history_to_file();
    
    // Free all allocated memory
    for (int i = 0; i < history_count; i++) {
        free(history[i]);
        history[i] = NULL;
    }
    history_count = 0;
}
