#ifndef MASTER_COMM_H
#define MASTER_COMM_H


#include "FreeRTOS.h"
#include "queue.h"

// Queue size and item size configuration
#define QUEUE_LENGTH 10
#define QUEUE_ITEM_SIZE sizeof(int)
#define TICK_TO_WAIT_SEND_MS 100
#define DELAY_SEND_MS 500

void initCommunicationMaster(QueueHandle_t myQueueHandle);
void sendMsgMaster(const void* data);
void reciveMsgMaster(void *data);

#endif // MASTER_COMM_H