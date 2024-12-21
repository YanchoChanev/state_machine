#include <stdio.h>
#include "FreeRTOS.h"
#include "logger.h"
#include "queue.h"
#include "slave_comm.h"
#include "task.h"

// constants for the queue
#define TICK_TO_WAIT_SEND_MS 100
#define DELAY_SEND_MS 10

// Queue handle for state signals.
static QueueHandle_t restartQueueHandle_ = NULL;
static QueueHandle_t stateQueueHandler_ = NULL;

/**
 * @brief Internal function to send data to the queue.
 * @param data Pointer to the data to send.
 * @param ticks_to_wait Maximum time to wait for space.
 * @return pdPASS if successful, pdFAIL otherwise.
 */
static BaseType_t queueSend(ChannelId channeId, const void *data, TickType_t ticks_to_wait)
{
    if(channeId == STATE_CHANNEL){
        if (stateQueueHandler_ == NULL)
        {
            logMessage(LOG_LEVEL_ERROR, "SlaveComm", "Queue handle is not initialized in queueSend");
            return pdFAIL;
        }
        return xQueueSend(stateQueueHandler_, data, ticks_to_wait);
    }else if(channeId == REST_CHANNEL){
        if (restartQueueHandle_ == NULL)
        {
            logMessage(LOG_LEVEL_ERROR, "SlaveComm", "Queue handle is not initialized in queueSend");
            return pdFAIL;
        }
        return xQueueSend(restartQueueHandle_, data, ticks_to_wait);
    }
}

/**
 * @brief Internal function to receive data from the queue.
 * @param data Pointer to store received data.
 * @param ticks_to_wait Maximum time to wait for data.
 * @return pdPASS if successful, pdFAIL otherwise.
 */
static BaseType_t queueReceive(ChannelId channeId, void *data, TickType_t ticks_to_wait)
{
    if(channeId == STATE_CHANNEL){
        if (stateQueueHandler_ == NULL)
        {
            logMessage(LOG_LEVEL_ERROR, "SlaveComm", "Queue handle is not initialized in queueReceive");
            return pdFAIL;
        }
        return xQueueReceive(stateQueueHandler_, data, ticks_to_wait);
    }else if(channeId == REST_CHANNEL){
        if (restartQueueHandle_ == NULL)
        {
            logMessage(LOG_LEVEL_ERROR, "SlaveComm", "Queue handle is not initialized in queueReceive");
            return pdFAIL;
        }
        return xQueueReceive(restartQueueHandle_, data, ticks_to_wait);
    }
}

RetVal initSlaveComm(QueueHandle_t stateQueueHandler){
    stateQueueHandler_ = stateQueueHandler;
    restartQueueHandle_ = xQueueCreate(10, sizeof(uint8_t));
}

// Sends a message to the slave queue.
RetVal sendMsgSlave(ChannelId channeId, const void *data)
{
    if (queueSend(channeId, data, pdMS_TO_TICKS(TICK_TO_WAIT_SEND_MS)) != pdPASS)
    {
        logMessage(LOG_LEVEL_ERROR, "SlaveComm", "Failed to send message to the queue");
        return RET_ERROR;
    }
    else
    {
        logMessage(LOG_LEVEL_DEBUG, "SlaveComm", "Message sent successfully");
        return RET_OK;
    }

    vTaskDelay(pdMS_TO_TICKS(DELAY_SEND_MS));
}

// Receives a message from the slave queue.
RetVal reciveMsgSlave(ChannelId channeId, void *data)
{
    if (queueReceive(channeId, data, portMAX_DELAY) == pdPASS)
    {
        logMessage(LOG_LEVEL_DEBUG, "SlaveComm", "Message received successfully");
        return RET_OK;
    }
    else
    {
        logMessage(LOG_LEVEL_ERROR, "SlaveComm", "Failed to receive message from the queue");
        return RET_ERROR;
    }
}