#include <stdio.h>
#include "FreeRTOS.h"
#include "logger.h"
#include "queue.h"
#include "master_comm.h"
#include "task.h"
#include "comm_cfg.h"

/**
 * @file master_comm.c
 * @brief Implements communication functionalities for the master system.
 *
 * This file defines functions for initializing communication queues,
 * sending messages, and receiving messages between the master and slave systems.
 */

/**
 * @brief Queue handle for state signals.
 */
static QueueHandle_t stateQueueHandle_ = NULL;

/**
 * @brief Internal function to send data to the queue.
 *
 * Ensures that data is properly sent to the queue with a timeout.
 *
 * @param data Pointer to the data to send.
 * @param ticks_to_wait Maximum time to wait for space.
 * @return pdPASS if successful, pdFAIL otherwise.
 */
static BaseType_t queueSend(const void *data, TickType_t ticks_to_wait) {
    if (stateQueueHandle_ == NULL) {
        printf("Queue handle is not initialized in queueSend\n");
        logMessage(LOG_LEVEL_ERROR, "MasterComm", "Queue handle is not initialized in queueSend");
        return pdFAIL;
    }
    return xQueueSend(stateQueueHandle_, data, ticks_to_wait);
}

/**
 * @brief Internal function to receive data from the queue.
 *
 * Ensures that data is properly received from the queue with a timeout.
 *
 * @param data Pointer to store received data.
 * @param ticks_to_wait Maximum time to wait for data.
 * @return pdPASS if successful, pdFAIL otherwise.
 */
static BaseType_t queueReceive(void *data, TickType_t ticks_to_wait) {
    if (stateQueueHandle_ == NULL) {
        logMessage(LOG_LEVEL_ERROR, "MasterComm", "Queue handle is not initialized in queueReceive");
        return pdFAIL;
    }
    return xQueueReceive(stateQueueHandle_, data, ticks_to_wait);
}

/**
 * @brief Initializes the master communication module.
 *
 * Sets up the communication queue for master-slave interactions.
 *
 * @param stateQueueHandle Queue handle for state communication.
 */
RetVal_t initMasterComm(QueueHandle_t stateQueueHandle) {
    if(stateQueueHandle == NULL) {
        logMessage(LOG_LEVEL_ERROR, "MasterComm", "Queue handle is NULL");
        return RET_ERROR;
    }

    stateQueueHandle_ = stateQueueHandle;
    return RET_OK;
}

/**
 * @brief Sends a message to the slave queue.
 *
 * Wraps the internal queueSend function to ensure proper message delivery.
 *
 * @param data Pointer to the data to send.
 * @return RET_OK if successful, RET_ERROR otherwise.
 */
RetVal_t sendMsgMaster(const void *data) {
    if (queueSend(data, pdMS_TO_TICKS(TICK_TO_WAIT_SEND_MS)) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "MasterComm", "Failed to send message to the queue");
        return RET_ERROR;
    }
    logMessage(LOG_LEVEL_DEBUG, "MasterComm", "Message sent successfully");
    vTaskDelay(pdMS_TO_TICKS(DELAY_SEND_MS));
    return RET_OK;
}

/**
 * @brief Receives a message from the slave queue.
 *
 * Waits for and retrieves a message from the communication queue.
 *
 * @param data Pointer to store the received data.
 * @return RET_OK if successful, RET_ERROR otherwise.
 */
RetVal_t reciveMsgMaster(void *data) {
    if (queueReceive(data, portMAX_DELAY) == pdPASS) {
        logMessage(LOG_LEVEL_DEBUG, "MasterComm", "Message received successfully");
        return RET_OK;
    }
    logMessage(LOG_LEVEL_ERROR, "MasterComm", "Failed to receive message from the queue");
    return RET_ERROR;
}
