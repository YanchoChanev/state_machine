#ifndef MASTER_HANDLER_H
#define MASTER_HANDLER_H

#include "FreeRTOS.h"
#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file master_handler.h
 * @brief Header file for Master Handler tasks.
 *
 * This file declares functions responsible for handling master communication
 * and status-check tasks. These tasks ensure proper communication with the
 * slave system and periodic status monitoring.
 */

/**
 * @brief Handles master communication tasks.
 *
 * This task is responsible for receiving messages from the slave system
 * and dispatching them to appropriate state handlers.
 *
 * @param args Pointer to task arguments (if any).
 */
void vMasterReciverHandler(void *args);

/**
 * @brief Handles master status check tasks.
 *
 * This task periodically retrieves the current state of the master system
 * and sends updates to the slave system.
 *
 * @param args Pointer to task arguments (if any).
 */
void vMasterSenderHandler(void *args);

#ifdef __cplusplus
}
#endif

#endif // MASTER_HANDLER_H
