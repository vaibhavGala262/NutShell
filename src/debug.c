#include "debug.h"
#include <string.h>

// Global debug level variable
debug_level_t g_debug_level = DEBUG_NONE;

void set_debug_level(debug_level_t level) {
    g_debug_level = level;
}

const char* debug_level_to_string(debug_level_t level) {
    switch (level) {
        case DEBUG_NONE:    return "NONE";
        case DEBUG_ERROR:   return "ERROR";
        case DEBUG_WARN:    return "WARN";
        case DEBUG_INFO:    return "INFO";
        case DEBUG_VERBOSE: return "VERBOSE";
        default:            return "UNKNOWN";
    }
}
