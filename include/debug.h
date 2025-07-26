#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

// Debug levels
typedef enum {
    DEBUG_NONE = 0,     // No debug output
    DEBUG_ERROR = 1,    // Only errors
    DEBUG_WARN = 2,     // Warnings and errors
    DEBUG_INFO = 3,     // General information
    DEBUG_VERBOSE = 4   // Detailed tracing
} debug_level_t;

// Global debug state
extern debug_level_t g_debug_level;

// Debug macros
#define DEBUG_ERROR(fmt, ...) \
    do { if (g_debug_level >= DEBUG_ERROR) \
         fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__); } while(0)

#define DEBUG_WARN(fmt, ...) \
    do { if (g_debug_level >= DEBUG_WARN) \
         fprintf(stderr, "[WARN] " fmt "\n", ##__VA_ARGS__); } while(0)

#define DEBUG_INFO(fmt, ...) \
    do { if (g_debug_level >= DEBUG_INFO) \
         fprintf(stderr, "[INFO] " fmt "\n", ##__VA_ARGS__); } while(0)

#define DEBUG_VERBOSE(fmt, ...) \
    do { if (g_debug_level >= DEBUG_VERBOSE) \
         fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__); } while(0)

// Convenience macros
#define DEBUG_PRINT(fmt, ...) DEBUG_VERBOSE(fmt, ##__VA_ARGS__)

// Function to set debug level
void set_debug_level(debug_level_t level);
const char* debug_level_to_string(debug_level_t level);

#endif
