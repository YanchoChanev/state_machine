#ifndef TCP_COMM_H
#define TCP_COMM_H

/**
 * @file TCP_comm.h
 * @brief Header file for the TCP communication module.
 *
 * This file contains the declaration for the TCP Echo Server Task,
 * which handles incoming TCP connections and echoes client messages.
 */

/**
 * @brief Start the TCP Echo Server Task.
 *
 * Initializes and starts a TCP server that listens for incoming client connections.
 * The server processes incoming messages and sends responses back to the clients
 * in an echo-like manner. This task runs indefinitely and handles communication
 * asynchronously.
 *
 * @note This function should be called from a FreeRTOS task context.
 */
void tcpEchoServerTask();

#endif // TCP_COMM_H
