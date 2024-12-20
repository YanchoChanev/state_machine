#include "slave_comm.h"
#include <stdio.h>

// Static Queue Handle
static QueueHandle_t myQueueHandle_ = NULL;

static BaseType_t queueSend(const void *data, TickType_t ticks_to_wait);
static BaseType_t queueReceive(void *data, TickType_t ticks_to_wait);

void initComunicationSlave(QueueHandle_t myQueueHandle){
    if (myQueueHandle_ == NULL) {
        myQueueHandle_ = myQueueHandle;
    }
}

void sendMsgSlave(const void* data){
    printf("Slave: Sending message\n");
    if (queueSend(data, pdMS_TO_TICKS(TICK_TO_WAIT_SEND_MS)) == pdPASS) {
        // Data sent successfully
    } else {
        // Failed to send data
    }

    vTaskDelay(pdMS_TO_TICKS(DELAY_SEND_MS));
}

void reciveMsgSlave(void *data) {
    if (xQueueReceive(myQueueHandle_, data, portMAX_DELAY) == pdTRUE) {
        printf("Slave: Received message\n");
    } else {
        printf("Slave: Failed to receive message\n");
    }
}

static BaseType_t queueSend(const void *data, TickType_t ticks_to_wait){
    if (myQueueHandle_ == NULL) {
        // Handle error: Queue not initialized
        return pdFAIL;
    }
    return xQueueSend(myQueueHandle_, data, ticks_to_wait);
}

static BaseType_t queueReceive(void *data, TickType_t ticks_to_wait){
    if (myQueueHandle_ == NULL) {
        // Handle error: Queue not initialized
        return pdFAIL;
    }
    return xQueueReceive(myQueueHandle_, data, ticks_to_wait);
}