#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "logger.h"
#include "slave_comm.h"
#include "slave_handler.h"
#include "slave_state_machine.h"
#include "master_state_machine.h"
#include "queue.h"

// Handles restart signals.
void vRestartHandler(void *args) {
    uint8_t signal;
    

    while (1) {
        if (reciveMsgSlave(REST_CHANNEL, &signal) == RET_OK) {
            logMessage(LOG_LEVEL_DEBUG, "SlaveHandler", "Received restart signal");
            if (signal == 1) {
                handelStatus(SLEEP);
                vTaskDelay(pdMS_TO_TICKS(10000));
                if (restartAllTasks() != RET_OK) {
                    logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Failed to restart all tasks");
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Observes and retrieves the slave's status.
void vSlaveStatusObservationHandler(void *args) {
    logMessage(LOG_LEVEL_INFO, "SlaveHandler", "vSlaveStatusObservationHandler created");
    QueueMessage data;

    while (1) {
        if (reciveMsgSlave(STATE_CHANNEL, &data) != RET_OK) {
            logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Failed to receive message");
        } else {
            logMessageFormatted(LOG_LEVEL_DEBUG, "SlaveHandler", "Message ID = %d, message state = %d", data.msg_id, data.state);
            data.msg_id = SLAVE_CURRENT_STATUS;
            data.state = getState();
            // printf("State: %d\n", data.state);
            if (sendMsgSlave(STATE_CHANNEL, &data) != RET_OK) {
                logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Failed to send message");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Handles communication tasks.
void vSlaveCommHandler(void *args) {
    logMessage(LOG_LEVEL_INFO, "SlaveHandler", "vSlaveCommHandler created");
    QueueMessage data;

    while (1) {
        if (reciveMsgSlave(STATE_CHANNEL, &data) == RET_OK) {
            logMessageFormatted(LOG_LEVEL_DEBUG, "SlaveHandler", "Message ID = %d, message state = %d", data.msg_id, data.state);
            if (data.msg_id == RESET_SLAVE) {
                handelStatus(SLEEP);
                uint8_t signal = 1;
                if (sendMsgSlave(REST_CHANNEL, &signal) != pdPASS) { // Send restart signal
                    logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Failed to send restart signal");
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Handles test tasks with user input.
void vSlaveTaskTestHandling(void *args) {
    char input[16];
    int number;
    logMessage(LOG_LEVEL_DEBUG, "SlaveHandler", "vSlaveTaskTestHandling created");

    while (1) {
        printf("Enter a number between 0 and 2: ");
        fflush(stdout);

        while (1) {
            if (fgets(input, sizeof(input), stdin) != NULL) {
                // Remove newline character if present
                input[strcspn(input, "\n")] = 0;

                // Convert input to an integer
                number = atoi(input);

                // Validate the number
                if (number >= 0 && number <= 2) {
                    logMessageFormatted(LOG_LEVEL_INFO, "SlaveHandler", "Valid input received: %d", number);
                    handelStatus(number);
                    break; // Exit the inner loop on valid input
                } else {
                    logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Invalid input. Please enter a number between 0 and 2.");
                }
            } else {
                // logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Error reading input.");
            }

            // Clear the input buffer
            while (getchar() != '\n' && getchar() != EOF);
            vTaskDelay(pdMS_TO_TICKS(100));
        }

        // Simulate task delay to prevent tight looping
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
