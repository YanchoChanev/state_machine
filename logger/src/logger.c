#include "logger.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

#define LOG_FILE "slave_log.txt"

void logMessage(const char* priority, const char* thread, const char* message) {
    FILE* logFile = fopen(LOG_FILE, "a");
    if (logFile == NULL) {
        perror("Failed to open log file");
        return;
    }

    time_t now = time(NULL);
    char* timestamp = ctime(&now);
    timestamp[strlen(timestamp) - 1] = '\0'; // Remove newline character

    fprintf(logFile, "[%s] [%s] [%s] %s\n", timestamp, priority, thread, message);
    fclose(logFile);
}
