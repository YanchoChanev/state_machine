#ifndef MASTER_HANDLER_H
#define MASTER_HANDLER_H
#include "FreeRTOS.h"
#include "queue.h"
/**
 * @brief Handles master communication tasks.
 * @param args Task arguments.
 */
void vMasterCommHandler(void *args);

/**
 * @brief Handles master status check tasks.
 * @param args Task arguments.
 */
void vMasterStatusCheckHandler(void *args);

#endif // MASTER_HANDLER_H