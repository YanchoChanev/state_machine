#include "FreeRTOS.h"
#include "queue.h"
#include "slave_restart_threads.h"
#include "slave_handler.h"
#include "logger.h"
#include "thread_handler_cfg.h"
#include "state_mashine_types.h"

/**
 * @file slave_restart_threads.c
 * @brief Manages task restarts and lifecycle in the slave system.
 *
 * This module provides functionality for restarting, recreating,
 * and managing tasks in a structured way using FreeRTOS.
 */

/**
 * @brief Array of task handlers with their metadata.
 *
 * Each task handler includes:
 * - Task ID
 * - Task function pointer
 * - Task name
 * - Task priority
 * - Task handle
 */
static TaskHandler taskHandlers_[SLAVE_TAKS_HANDLERS_SIZE] = {
    {SLAVE_STATUS_OBSERVATION_HANDLER_ID, vSlaveStatusObservationHandler, "SlaveStatusObservationHandler", 
    TASTK_PRIO_SLAVE_STATUS_OBSERVATION_HANDLING, NULL},
    {TCP_ECHO_SERVER_TASK, vTCPEchoServerTask, "TCPEchoServerTask", TASTK_PRIO_ECHO_SERVER_HANDLER, NULL},
};

/**
 * @brief Deletes all tasks in the task handler array.
 *
 * Iterates through all task handlers and deletes any tasks
 * with non-NULL handles to ensure proper cleanup.
 */
static void deleteAllTasks() {
    for (uint8_t i = 0; i < SLAVE_TAKS_HANDLERS_SIZE; i++) {
        if (taskHandlers_[i].taskHandler != NULL) {
            logMessageFormatted(LOG_LEVEL_INFO, "SlaveRestartThread", "Deleting task %d", i);
            vTaskDelete(taskHandlers_[i].taskHandler);
            taskHandlers_[i].taskHandler = NULL;
        }
    }
}

/**
 * @brief Recreates all tasks in the task handler array.
 *
 * Iterates through all task handlers and attempts to recreate tasks
 * with their corresponding function, name, priority, and handle.
 *
 * @return RET_OK on success, RET_ERROR on failure.
 */
static RetVal_t recreateAllTasks() {
    for (uint8_t i = 0; i < SLAVE_TAKS_HANDLERS_SIZE; i++) {
        logMessageFormatted(LOG_LEVEL_INFO, "SlaveRestartThread", "Recreating task %d", i);
        if (xTaskCreate(taskHandlers_[i].taskFunction, taskHandlers_[i].taskName,
                        configMINIMAL_STACK_SIZE * 16, NULL, taskHandlers_[i].taskPrio, 
                        &taskHandlers_[i].taskHandler) != pdPASS) {
            logMessageFormatted(LOG_LEVEL_ERROR, "SlaveRestartThread", "Failed to create task %d", i);
            return RET_ERROR;
        } else {
            logMessageFormatted(LOG_LEVEL_INFO, "SlaveRestartThread", "Task %d created successfully", i);
        }
    }
    return RET_OK;
}

/**
 * @brief Restarts all tasks in the system.
 *
 * This function deletes all currently running tasks and recreates them.
 * It also resets the system state to SLEEP upon successful recreation.
 *
 * @return RET_OK if all tasks are restarted successfully, RET_ERROR otherwise.
 */
RetVal_t restartAllTasks() {
    logMessage(LOG_LEVEL_INFO, "SlaveRestartThread", "Restarting all tasks");
    deleteAllTasks();

    if (recreateAllTasks() != RET_OK) {
        logMessage(LOG_LEVEL_ERROR, "SlaveRestartThread", "Failed to recreate tasks");
        return RET_ERROR;
    }

    if (handelStatus(SLAVE_STATE_SLEEP) != RET_OK) {
        logMessage(LOG_LEVEL_ERROR, "SlaveRestartThread", "Failed to set state ACTIVE");
        return RET_ERROR;
    }
    return RET_OK;
}

/**
 * @brief Sets the task handlers for the slave system.
 *
 * Assigns external task handles to the internal task handler array.
 *
 * @param taskHandlers Array of task handles to assign.
 */
void setTaskHandlers(TaskHandle_t *taskHandlers) {
    for (int i = 0; i < SLAVE_TAKS_HANDLERS_SIZE; i++) {
        taskHandlers_[i].taskHandler = taskHandlers[i];
    }
}   
