#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "types.h"
#include "master_handler.h"
#include "master_comm.h"
#include "master_state_machine.h"
#include "logger.h"

// Handles master communication tasks.
void vMasterCommHandler(void *args) {
    logMessage(LOG_LEVEL_INFO, "MasterHandler", "Master communication handler started");
    QueueMessage data;

    while (1) {
        // printf("Master communication handler\n");
        if (reciveMsgMaster(&data) != RET_OK) {
            logMessage(LOG_LEVEL_ERROR, "MasterHandler", "Failed to receive message");
        }

        logMessageFormatted(LOG_LEVEL_DEBUG, "MasterHandler", "Message ID = %d, message state = %d", data.msg_id, data.state);
        // printf("Message ID = %d, message state = %d\n", data.msg_id, data.state);
        if (stateDispatcher(data) != RET_OK) {
            logMessage(LOG_LEVEL_ERROR, "MasterHandler", "Failed to dispatch state");
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
    // vTaskDelay(pdMS_TO_TICKS(100));
}

//Handles master status check tasks.
void vMasterStatusCheckHandler(void *args) {
    logMessage(LOG_LEVEL_INFO, "MasterHandler", "Master status check handler started");
    QueueMessage data = {SLAVE_CURRENT_STATUS, 0};

    while (1) {
        if (sendMsgMaster(&data) != RET_OK) {
            logMessage(LOG_LEVEL_ERROR, "MasterHandler", "Failed to send message");
        }

        logMessage(LOG_LEVEL_DEBUG, "MasterHandler", "Sent status check message to slave");
        vTaskDelay(pdMS_TO_TICKS(90));
    }
    // vTaskDelay(pdMS_TO_TICKS(100));
}
