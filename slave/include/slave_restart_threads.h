#ifndef SLAVE_RESTART_THREADS_H
#define SLAVE_RESTART_THREADS_H

#include "FreeRTOS.h"
#include "task.h"
#include "types.h"

/**
 * @file slave_restart_threads.h
 * @brief Header file for managing task restart functionality in the slave system.
 *
 * This file defines structures and function declarations for managing and restarting
 * tasks in a FreeRTOS-based slave system. It includes task metadata structures,
 * task restart mechanisms, and task handler management.
 */

/**
 * @brief Structure to manage task-related metadata.
 *
 * This structure holds essential information about a task, including its identifier,
 * function pointer, name, priority, and handler reference.
 */
typedef struct {
    uint8_t id;                  ///< Task identifier.
    TaskFunction_t taskFunction; ///< Pointer to the task function.
    const char *taskName;        ///< Name of the task.
    uint8_t taskPrio;            ///< Priority assigned to the task.
    TaskHandle_t taskHandler;    ///< Task handler reference.
} TaskHandler;

/**
 * @brief Number of task handlers defined.
 *
 * This macro defines the total number of task handlers that can be managed
 * by the slave restart system.
 */
#define TAKS_HADLERS_SIZE 2

/**
 * @brief Restart all tasks managed by the task handlers.
 *
 * Iterates through all registered task handlers, terminates the existing tasks,
 * and recreates them to ensure proper system functionality.
 *
 * @return RET_OK if all tasks were successfully restarted, RET_ERROR otherwise.
 */
RetVal restartAllTasks();

/**
 * @brief Retrieve the restart queue handle.
 *
 * Provides access to the restart queue used for signaling task restarts.
 *
 * @param restartQueueHandle Pointer to store the queue handle.
 * @return RET_OK if the queue handle was successfully retrieved, RET_ERROR otherwise.
 */
RetVal getRestartQueue(QueueHandle_t restartQueueHandle);

/**
 * @brief Set the task handlers for task management.
 *
 * Registers an array of task handlers that will be used for managing and controlling tasks
 * within the slave system.
 *
 * @param taskHandlers Pointer to an array of task handlers.
 */
void setTaskHandlers(TaskHandle_t *taskHandlers);

/**
 * @brief Initialize the reset handler for task management.
 *
 * Sets up the reset queue and prepares the handler for managing task restarts.
 *
 * @param resetQueueHandler Queue handle for managing task resets.
 * @return RET_OK if initialization succeeded, RET_ERROR otherwise.
 */
RetVal initResetHandler(QueueHandle_t resetQueueHandler);

#endif // SLAVE_RESTART_THREADS_H
