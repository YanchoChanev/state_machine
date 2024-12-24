#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "logger.h"
#include "slave_comm.h"
#include "slave_handler.h"
#include "TCP_comm.h"
#include "slave_state_machine.h"
#include "slave_restart_threads.h"
#include "queue.h"

#include "thread_handler_cfg.h"

/**
 * @file slave_handler.c
 * @brief Handles various tasks related to slave communication, observation, and TCP communication.
 *
 * This file contains task handlers for managing restart signals, observing slave status,
 * and managing TCP echo server tasks.
 */

/**
 * @brief Queue handle for reset handler task.
 *
 * Used for receiving restart signals from the reset queue.
 */
QueueHandle_t resetHandlerTask_ = NULL;

/**
 * @brief Handles restart signals for the slave system.
 *
 * This task listens for restart signals via the REST_CHANNEL. Upon receiving a valid restart signal,
 * it sets the system state to SLEEP, waits for a predefined delay, and then attempts to restart all tasks.
 *
 * @param args Pointer to task arguments (used for passing reset queue handle).
 */
void vRestartHandler(void *args) {
    uint8_t signal = 0;
    resetHandlerTask_ = (QueueHandle_t)args;

    if (resetHandlerTask_ != NULL) {
        while (1) {
            // Ensure the queue handle is valid
            configASSERT(resetHandlerTask_);
            
            // Receive a restart signal from the REST_CHANNEL
            if (xQueueReceive(resetHandlerTask_, &signal, portMAX_DELAY) == pdPASS) {
                logMessage(LOG_LEVEL_DEBUG, "SlaveHandler", "Received restart signal");
                vTaskDelay(pdMS_TO_TICKS(DELAY_BEFORE_RESTART));
                
                if (signal == 1) {
                    if (restartAllTasks() != RET_OK) {
                        logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Failed to restart all tasks");
                    }
                }
            }
            
            // Delay to prevent task starvation
            vTaskDelay(pdMS_TO_TICKS(TASTK_TIME_SLAVE_RESTAT_STATUS));
        }
    } else {
        logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Reset handler is NULL");
    }
}

/**
 * @brief Observes and retrieves the slave's status.
 *
 * This task listens for messages on the STATE_CHANNEL. Upon receiving a valid message,
 * it retrieves the current slave state and sends an updated status message back via the STATE_CHANNEL.
 *
 * @param args Pointer to task arguments (unused in this implementation).
 */
void vSlaveStatusObservationHandler(void *args) {
    MasterStates data = MASTESR_STATE_IDLE;
    SlaveStates sendData = SLAVE_STATE_MAX;
    
    while (1) {
        // Receive a message from the STATE_CHANNEL
        if (reciveMsgSlave(&data) != RET_OK) {
            logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Failed to receive message");
        } else {
            if (getState(&sendData) != RET_ERROR) {
                // Check if state has changed
                if (sendData != data) {
                    if (sendMsgSlave(&sendData) != RET_OK) {
                        logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Failed to send updated status message");
                    }
                } else if (data == MASTESR_STATE_ERROR) {
                    // Handle RESET state if error occurs
                    handelStatus(SLAVE_INPUT_STATE_ERROR_OR_RESET);
                    if (sendMsgSlave(&sendData) != RET_OK) {
                        logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Failed to send reset status message");
                    }
                }
            }
        }
        
        // Delay to prevent task starvation
        vTaskDelay(pdMS_TO_TICKS(TASTK_TIME_SLAVE_STATUS_OBSERVATION_HANDLING));
    }
}

/**
 * @brief TCP Echo Server Task.
 *
 * This task starts and manages the TCP Echo Server, allowing it to listen for incoming
 * TCP connections and echo back received data.
 *
 * @param args Pointer to task arguments (unused in this implementation).
 */
void vTCPEchoServerTask(void *args) {
    logMessage(LOG_LEVEL_INFO, "SlaveHandler", "vTCPEchoServerTask started");
    tcpEchoServerTask();
}
