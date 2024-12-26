#ifndef TCO_COMM_CFG_H
#define TCO_COMM_CFG_H

/**
 * @file tco_comm_cfg.h
 * @brief Configuration file for TCP communication.
 *
 * This file defines configuration macros used for setting up TCP communication
 * parameters, including port number, buffer sizes, and connection limits.
 */

/**
 * @brief Port number for TCP communication.
 *
 * Defines the port number on which the TCP server listens for incoming connections.
 */
#define PORT 5003

/**
 * @brief Maximum number of simultaneous connection requests.
 *
 * Limits the number of incoming connection requests that can be queued.
 */
#define CONNECTION_REQUESTS 5

/**
 * @brief Maximum parsed inputs per TCP client message.
 *
 * Defines how many separate inputs are expected to be parsed from a single
 * TCP client message.
 */
#define TCP_MESSAGE_CLIENT_PARCED_INPUTS 2

/**
 * @brief TCP communication buffer size.
 *
 * Sets the size of the buffer used for receiving and sending TCP messages.
 */
#define TCP_BUFFER_SIZE 1024

/**
 * @brief Socket option value.
 *
 * Defines the default value for socket options (e.g., SO_REUSEADDR).
 */
#define OPT_VALUE 1

#endif // TCO_COMM_CFG_H
