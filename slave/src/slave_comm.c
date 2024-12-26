#include <stdio.h>
#include "FreeRTOS.h"
#include "logger.h"
#include "queue.h"
#include "slave_comm.h"
#include "task.h"
#include "comm_cfg.h"

/**
 * @file slave_comm.c
 * @brief Handles inter-task communication for the slave system using FreeRTOS queues.
 *
 * This file provides functions for sending and receiving messages between tasks
 * using state and restart communication channels.
 */

/**
 * @brief Queue handle for managing state messages.
 
 * Used for inter-task communication within the slave system.
 */
static QueueHandle_t stateQueueHandler_ = NULL;

/**
 * @brief Internal function to send data to a specified queue.
 *
 * Sends data to the designated queue based on the provided channel identifier.
 * Logs an error if the queue is not initialized.
 *
 * @param channeId Channel identifier (STATE_CHANNEL or REST_CHANNEL).
 * @param data Pointer to the data to send.
 * @param ticks_to_wait Maximum time to wait for space in the queue.
 * @return pdPASS if the data was successfully sent, pdFAIL otherwise.
 */
static BaseType_t queueSend(const void *data, TickType_t ticks_to_wait) {
    if (stateQueueHandler_ == NULL) {
        logMessage(LOG_LEVEL_ERROR, "SlaveComm", "Queue handle is not initialized in queueSend (STATE_CHANNEL)");
        return pdFAIL;
    }
    return xQueueSend(stateQueueHandler_, data, ticks_to_wait);
}

/**
 * @brief Internal function to receive data from a specified queue.
 *
 * Receives data from the designated queue based on the provided channel identifier.
 * Logs an error if the queue is not initialized.
 *
 * @param channeId Channel identifier (STATE_CHANNEL or REST_CHANNEL).
 * @param data Pointer to store the received data.
 * @param ticks_to_wait Maximum time to wait for data in the queue.
 * @return pdPASS if data was successfully received, pdFAIL otherwise.
 */
static BaseType_t queueReceive(void *data, TickType_t ticks_to_wait) {
    if (stateQueueHandler_ == NULL) {
        logMessage(LOG_LEVEL_ERROR, "SlaveComm", "Queue handle is not initialized in queueReceive (STATE_CHANNEL)");
        return pdFAIL;
    }
    return xQueueReceive(stateQueueHandler_, data, ticks_to_wait);
}

/**
 * @brief Initializes the communication queue for the slave system.
 *
 * Sets up the queue for managing state messages between tasks.
 *
 * @param stateQueueHandler Handle to the state queue.
 * @return RET_OK if initialization succeeded, RET_ERROR otherwise.
 */
RetVal_t initSlaveComm(QueueHandle_t stateQueueHandler) {
    if (stateQueueHandler == NULL) {
        logMessage(LOG_LEVEL_ERROR, "SlaveComm", "Failed to initialize state queue handler");
        return RET_ERROR;
    }

     stateQueueHandler_ = stateQueueHandler;

    return RET_OK;
}

/**
 * @brief Sends a message to the specified slave communication queue.
 *
 * Wraps the internal queueSend function and ensures the message is successfully sent.
 * Logs appropriate errors on failure.
 *
 * @param channeId Channel identifier (STATE_CHANNEL or REST_CHANNEL).
 * @param data Pointer to the data to send.
 * @return RET_OK if the message was successfully sent, RET_ERROR otherwise.
 */
RetVal_t sendMsgSlave(const void *data) {
    if (queueSend(data, pdMS_TO_TICKS(TICK_TO_WAIT_SEND_MS)) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "SlaveComm", "Failed to send message to the queue");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_DEBUG, "SlaveComm", "Message sent successfully");
        vTaskDelay(pdMS_TO_TICKS(DELAY_SEND_MS));
        return RET_OK;
    }
}

/**
 * @brief Receives a message from the specified slave communication queue.
 *
 * Wraps the internal queueReceive function and ensures the message is successfully received.
 * Logs appropriate errors on failure.
 *
 * @param channeId Channel identifier (STATE_CHANNEL or REST_CHANNEL).
 * @param data Pointer to store the received data.
 * @return RET_OK if a message was successfully received, RET_ERROR otherwise.
 */
RetVal_t reciveMsgSlave(void *data) {
    if (queueReceive(data, portMAX_DELAY) == pdPASS) {
        logMessage(LOG_LEVEL_DEBUG, "SlaveComm", "Message received successfully");
        return RET_OK;
    } else {
        logMessage(LOG_LEVEL_ERROR, "SlaveComm", "Failed to receive message from the queue");
        return RET_ERROR;
    }
}
