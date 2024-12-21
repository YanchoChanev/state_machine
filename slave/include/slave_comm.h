#ifndef SLAVE_COMM_H
#define SLAVE_COMM_H

#include "types.h"
#include "FreeRTOS.h"
#include "queue.h"

typedef enum {
    STATE_CHANNEL,
    REST_CHANNEL
}ChannelId;

/**
 * @brief Initializes the restart queue.
 * @return RET_OK on success, RET_ERROR on failure.
 */
RetVal initSlaveComm(QueueHandle_t stateQueueHandler);


/**
 * @brief Sends a message to the slave queue.
 * @param data Pointer to the data to send.
 */
RetVal sendMsgSlave(ChannelId channeId, const void *data);

/**
 * @brief Receives a message from the slave queue.
 * @param data Pointer to store the received data.
 * @return RET_OK if successful, RET_ERROR otherwise.
 */
RetVal reciveMsgSlave(ChannelId channeId, void *data);

#endif // SLAVE_COMM_H
