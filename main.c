/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h> // Include pthread.h to use PTHREAD_STACK_MIN

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Include the task headers */
#include "master_handler.h"
#include "master_state_machine.h"
#include "master_comm.h"
#include "slave_handler.h"
#include "slave_restart_threads.h"
#include "slave_state_machine.h"
#include "slave_comm.h"
#include "types.h"
#include "logger.h"
#include "thread_handler_cfg.h"

/**
 * @file main.c
 * @brief Main file for initializing and starting FreeRTOS tasks.
 *
 * This file sets up communication queues, initializes components,
 * and creates master and slave tasks before starting the FreeRTOS scheduler.
 */

/* Define the maximum number of messages and the maximum message size */
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE sizeof(uint8_t)

/**
 * @brief Queue handles for communication and reset tasks.
 */
static QueueHandle_t xQueue = NULL;
static QueueHandle_t resetQueueHandler = NULL;

/**
 * @brief Initializes essential components such as queues and state machines.
 *
 * @return RET_OK on success, RET_ERROR on failure.
 */
static RetVal_t initComponents() {
    xQueue = xQueueCreate(MAX_MESSAGES, MAX_MSG_SIZE);
    resetQueueHandler = xQueueCreate(MAX_MESSAGES, sizeof(uint8_t));

    if (xQueue == NULL) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create queue");
        return RET_ERROR;
    }
    logMessage(LOG_LEVEL_INFO, "Main", "Queue created successfully");

    if (initStateSemaphoreMaster() != RET_OK) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Init State Semaphore Master failed");
        return RET_ERROR;
    }

    if (initStateMachineSlave(resetQueueHandler) != RET_OK) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Init State Machine Slave failed");
        return RET_ERROR;
    }

    if(initMasterComm(xQueue) != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Init Master Comm failed");
        return RET_ERROR;
    }

    if(initSlaveComm(xQueue) != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Init Slave Comm failed");
        return RET_ERROR;
    }

    return RET_OK;
}

/**
 * @brief Creates master tasks for communication and status handling.
 *
 * @return RET_OK on success, RET_ERROR on failure.
 */
static RetVal_t creatMasterTasks() {
    if (xTaskCreate(vMasterCommHandler, "MasterTask", configMINIMAL_STACK_SIZE * 4, NULL,
                    TASTK_PRIO_MASTER_COMM_HANDLER, NULL) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create MasterTask");
        return RET_ERROR;
    }
    logMessage(LOG_LEVEL_INFO, "Main", "MasterTask created successfully");

    if (xTaskCreate(vMasterStatusCheckHandler, "MasterStatusCheckHandler", configMINIMAL_STACK_SIZE * 4, NULL,
                    TASTK_PRIO_MASTER_STATUS_CHECK_HANDLER, NULL) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create MasterStatusCheckHandler");
        return RET_ERROR;
    }
    logMessage(LOG_LEVEL_INFO, "Main", "MasterStatusCheckHandler created successfully");

    return RET_OK;
}

/**
 * @brief Creates slave tasks for observation, TCP echo, and restart handling.
 *
 * @return RET_OK on success, RET_ERROR on failure.
 */
static RetVal_t creatSlaveTasks() {
    TaskHandle_t slaveTaskHandels[SLAVE_TAKS_HANDLERS_SIZE] = {NULL, NULL};

    if (xTaskCreate(vSlaveStatusObservationHandler, "SlaveStatusObservationHandler", configMINIMAL_STACK_SIZE * 4, resetQueueHandler,
                    TASTK_PRIO_SLAVE_STATUS_OBSERVATION_HANDLING, &slaveTaskHandels[SLAVE_STATUS_OBSERVATION_HANDLER_ID]) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create vSlaveStatusObservationHandler");
        return RET_ERROR;
    }
    logMessage(LOG_LEVEL_INFO, "Main", "vSlaveStatusObservationHandler created successfully");

    if (xTaskCreate(vTCPEchoServerTask, "TCPEchoServerTask", configMINIMAL_STACK_SIZE * 16, NULL,
                    TASTK_PRIO_ECHO_SERVER_HANDLER, &slaveTaskHandels[TCP_ECHO_SERVER_TASK]) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create vTCPEchoServerTask");
        return RET_ERROR;
    }
    logMessage(LOG_LEVEL_INFO, "Main", "vTCPEchoServerTask created successfully");

    if (xTaskCreate(vRestartHandler, "RestartSlave", configMINIMAL_STACK_SIZE * 16, resetQueueHandler,
                    TASTK_PRIO_SLAVE_RESTAT_STATUS, NULL) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create vRestartHandler");
        return RET_ERROR;
    }
    logMessage(LOG_LEVEL_INFO, "Main", "vRestartHandler created successfully");

    setTaskHandlers(&slaveTaskHandels[0]);
    return RET_OK;
}

/**
 * @brief Main entry point of the program.
 *
 * Initializes components, creates tasks, and starts the FreeRTOS scheduler.
 */
int main(void) {

    if (initComponents() != RET_OK) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Init Components failed");
        return 1;
    }

    if (creatSlaveTasks() != RET_OK) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Create Slave Tasks failed");
        return 1;
    }

    if (creatMasterTasks() != RET_OK) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Create Master Tasks failed");
        return 1;
    }

    vTaskStartScheduler();
    return 0;
}
