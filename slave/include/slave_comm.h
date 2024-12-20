#ifndef SLAVE_COMM_H
#define SLAVE_COMM_H

#include "FreeRTOS.h"
#include "queue.h"

// Queue size and item size configuration
#define QUEUE_LENGTH 10
#define QUEUE_ITEM_SIZE sizeof(int)
#define TICK_TO_WAIT_SEND_MS 100
#define DELAY_SEND_MS 500

void initComunicationSlave(QueueHandle_t myQueueHandle);
void sendMsgSlave(const void* data);
void reciveMsgSlave(void *data);

#endif // SLAVE_COMM_H