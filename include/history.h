#ifndef HISTORY_H
#define HISTORY_H

#define MAX_HISTORY 1000
#define HISTORY_FILE ".nutshell_history"

// Initialize history system (loads from file)
void init_history(void);

// Add command to in-memory history
void add_to_history(const char *cmd);

// Print current session history
void print_history(void);

// Save all history to file (called on exit/signal)
void save_history_to_file(void);

// Load history from file on startup
void load_history_from_file(void);

// Cleanup history (free memory)
void cleanup_history(void);

#endif
