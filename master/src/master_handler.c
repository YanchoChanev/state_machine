#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "types.h"
#include "master_handler.h"
#include "master_comm.h"
#include "master_state_machine.h"
#include "logger.h"
#include "thread_handler_cfg.h"

/**
 * @file master_handler.c
 * @brief Implements communication and status tasks for the master system.
 *
 * This file defines tasks responsible for handling communication with the slave
 * system and monitoring the master's status. It ensures proper message handling
 * and state management.
 */

/**
 * @brief Handles master communication tasks.
 *
 * This task listens for incoming messages from the slave system, processes them,
 * and dispatches the appropriate state handlers based on the received data.
 *
 * @param args Pointer to task arguments (unused in this implementation).
 */
void vMasterCommHandler(void *args) {
    SlaveStates data = SLAVE_STATE_MAX;
    MasterStates curentData = MASTESR_STATE_MAX;

#ifndef UNIT_TEST
    while(1){
#endif
        if (reciveMsgMaster(&data) != RET_OK) {
            logMessage(LOG_LEVEL_ERROR, "MasterHandler", "Failed to receive message");
        }

        if(data != curentData){
            if (stateDispatcher(data) != RET_OK) {
                logMessage(LOG_LEVEL_ERROR, "MasterHandler", "Failed to handle status");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(TASTK_TIME_MASTER_COMM_HANDLER));
#ifndef UNIT_TEST
    }
#endif
}

/**
 * @brief Handles master status check tasks.
 *
 * This task periodically retrieves the current state of the master system
 * and sends it to the slave system via the communication channel.
 *
 * @param args Pointer to task arguments (unused in this implementation).
 */
void vMasterStatusCheckHandler(void *args) {
    MasterStates currentState = MASTESR_STATE_MAX;

#ifndef UNIT_TEST
    while(1){
#endif
        (void)getCurrentState(&currentState);

        if (sendMsgMaster(&currentState) != RET_OK) {
            logMessage(LOG_LEVEL_ERROR, "MasterHandler", "Failed to send message");
        }
        vTaskDelay(pdMS_TO_TICKS(TASTK_TIME_MASTER_STATUS_CHECK_HANDLER));
#ifndef UNIT_TEST
    }
#endif
}
