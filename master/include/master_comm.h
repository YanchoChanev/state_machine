#ifndef SLAVE_COMM_H
#define SLAVE_COMM_H

#include "types.h"
#include "FreeRTOS.h"
#include "queue.h"

void initMasterComm(QueueHandle_t stateQueueHandle_);

/**
 * @brief Sends a message to the slave queue.
 * @param data Pointer to the data to send.
 */
RetVal sendMsgMaster(const void *data);

/**
 * @brief Receives a message from the slave queue.
 * @param data Pointer to store the received data.
 * @return RET_OK if successful, RET_ERROR otherwise.
 */
RetVal reciveMsgMaster(void *data);

#endif // SLAVE_COMM_H
