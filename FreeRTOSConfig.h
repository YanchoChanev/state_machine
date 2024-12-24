#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/**
 * @file freertos_config.h
 * @brief FreeRTOS configuration file.
 *
 * This file defines configuration parameters for FreeRTOS, including memory management
 * and task scheduling settings.
 */

/**
 * @brief Total heap size configuration.
 *
 * Defines the total available heap memory size for the FreeRTOS system.
 * The heap is used for dynamic memory allocation for tasks, queues, and other kernel objects.
 */
#define configTOTAL_HEAP_SIZE    ((size_t)(20 * 1024)) ///< Heap size set to 20 KB.

/**
 * @brief Enable time slicing for task scheduling.
 *
 * Allows tasks of equal priority to share CPU time in a round-robin manner.
 * This improves task fairness and prevents starvation.
 */
#define configUSE_TIME_SLICING 1

#endif /* FREERTOS_CONFIG_H */
