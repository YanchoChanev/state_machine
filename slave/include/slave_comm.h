#ifndef SLAVE_COMM_H
#define SLAVE_COMM_H

#include "types.h"
#include "FreeRTOS.h"
#include "queue.h"

/**
 * @file slave_comm.h
 * @brief Header file for the Slave Communication module.
 *
 * This file contains declarations for functions that manage inter-task communication
 * within the slave system using FreeRTOS queues. It provides mechanisms for
 * initializing communication channels, sending messages, and receiving messages.
 */

/**
 * @brief Initialize the slave communication module.
 *
 * Sets up the necessary communication queues for managing inter-task communication.
 * This function ensures proper initialization of state communication channels.
 *
 * @param stateQueueHandler Queue handle for state communication.
 * @return RET_OK if initialization was successful, RET_ERROR otherwise.
 */
RetVal initSlaveComm(QueueHandle_t stateQueueHandler);

/**
 * @brief Send a message through the specified communication channel.
 *
 * Sends data over the specified communication channel. The channel identifier
 * determines which queue the message will be sent to.
 *
 * @param channeId Identifier of the communication channel.
 * @param data Pointer to the data to be sent.
 * @return RET_OK if the message was successfully sent, RET_ERROR otherwise.
 */
RetVal sendMsgSlave(const void *data);

/**
 * @brief Receive a message from the specified communication channel.
 *
 * Receives data from the specified communication channel. The function blocks
 * until data is available or a timeout occurs.
 *
 * @param channeId Identifier of the communication channel.
 * @param data Pointer to store the received data.
 * @return RET_OK if a message was successfully received, RET_ERROR otherwise.
 */
RetVal reciveMsgSlave(void *data);

#endif // SLAVE_COMM_H
