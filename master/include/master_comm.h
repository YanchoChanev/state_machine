#ifndef SLAVE_COMM_H
#define SLAVE_COMM_H

#include "types.h"
#include "FreeRTOS.h"
#include "queue.h"

/**
 * @file slave_comm.h
 * @brief Header file for Slave Communication module.
 *
 * This file declares functions for initializing communication queues,
 * sending messages to, and receiving messages from the slave system.
 */

/**
 * @brief Initializes the master communication module.
 *
 * Sets up the communication queue for master-slave interactions.
 *
 * @param stateQueueHandle_ Queue handle for state communication.
 */
RetVal_t initMasterComm(QueueHandle_t stateQueueHandle_);

/**
 * @brief Sends a message to the slave queue.
 *
 * Wraps the internal queue send mechanism to ensure proper delivery of messages
 * to the slave system.
 *
 * @param data Pointer to the data to send.
 * @return RET_OK if the message was successfully sent, RET_ERROR otherwise.
 */
RetVal_t sendMsgMaster(const void *data);

/**
 * @brief Receives a message from the slave queue.
 *
 * Waits for and retrieves a message from the communication queue.
 *
 * @param data Pointer to store the received data.
 * @return RET_OK if a message was successfully received, RET_ERROR otherwise.
 */
RetVal_t reciveMsgMaster(void *data);

#endif // SLAVE_COMM_H
