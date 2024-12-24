#ifndef COMM_CFG_H
#define COMM_CFG_H

/**
 * @file comm_cfg.h
 * @brief Configuration file for communication parameters.
 *
 * This file defines timing parameters for communication tasks, including
 * message sending delays and timeout values for queue operations.
 */

/**
 * @brief Time to wait for space in the queue when sending a message (in ms).
 *
 * Defines the maximum amount of time (in milliseconds) to wait for space
 * to become available in the communication queue when sending a message.
 */
#define TICK_TO_WAIT_SEND_MS 100

/**
 * @brief Delay after sending a message (in ms).
 *
 * Specifies a delay (in milliseconds) after successfully sending a message
 * to allow communication systems to stabilize.
 */
#define DELAY_SEND_MS 10

#endif // COMM_CFG_H
