#ifndef SLAVE_HANDLER_H
#define SLAVE_HANDLER_H

#include "FreeRTOS.h"
#include "task.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file slave_handler.h
 * @brief Header file for managing slave handler tasks.
 *
 * This file contains declarations for task handler functions and structures
 * used to manage various tasks in the slave system, including status observation,
 * task restarting, and TCP communication.
 */

/**
 * @brief Enum defining task IDs.
 */
typedef enum {
    SLAVE_STATUS_OBSERVATION_HANDLER_ID, ///< Status observation handler ID.
    TCP_ECHO_SERVER_TASK                 ///< Test task handler ID.
} TasksId;

/**
 * @brief Set task handlers for task management.
 *
 * Registers an array of task handlers to manage tasks efficiently
 * within the slave system.
 *
 * @param taskHandlers Pointer to an array of task handlers.
 */
void setTaskHandlers(TaskHandle_t *taskHandlers);

/**
 * @brief Status observation handler task function.
 *
 * Monitors and logs the status of the slave system. It listens for status
 * updates and ensures appropriate state transitions when necessary.
 *
 * @param args Pointer to task arguments (can be used to pass parameters).
 */
void vSlaveStatusObservationHandler(void *args);

/**
 * @brief Restart handler task function.
 *
 * Listens for restart signals, processes them, and triggers the restart
 * sequence for all managed tasks.
 *
 * @param args Pointer to task arguments (can be used to pass parameters).
 */
void vRestartHandler(void *args);

/**
 * @brief TCP Echo server task function.
 *
 * Initializes and manages a TCP echo server task. It listens for incoming
 * TCP connections, processes client messages, and echoes responses.
 *
 * @param args Pointer to task arguments (can be used to pass parameters).
 */
void vTCPEchoServerTask(void *args);

#ifdef __cplusplus
}
#endif

#endif // SLAVE_HANDLER_H
