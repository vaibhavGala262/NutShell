// src/history.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "history.h"

static char *history[MAX_HISTORY];
static int history_count = 0;

void add_to_history(const char *cmd) {
    if (history_count < MAX_HISTORY) {
        history[history_count++] = strdup(cmd);
    }
}

void print_history(void) {
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i + 1, history[i]);
    }
}
