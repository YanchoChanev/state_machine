#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "logger.h"
#include "FreeRTOS.h"
#include "task.h"
#include "slave_state_machine.h"
#include "slave_TCP_comm_cfg.h"
#include "thread_handler_cfg.h"



/**
 * @file tcp_server.c
 * @brief Implementation of a TCP Echo Server using FreeRTOS.
 *
 * This file manages TCP socket creation, configuration, and client communication.
 * It handles incoming client connections, processes messages, and supports state transitions.
 */

/**
 * @brief Create a TCP socket.
 *
 * Initializes a TCP socket for communication.
 *
 * @param server_fd Pointer to the server socket file descriptor.
 * @param port Port number to bind the socket.
 * @return RET_OK on success, RET_ERROR on failure.
 */
static RetVal_t createSocket(int32_t* server_fd, int16_t port){
    *server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*server_fd < 0) {
        logMessageFormatted(LOG_LEVEL_ERROR, "TCPComm", "Socket creation failed for port %d, with error: %s", 
                            port, strerror(errno));
        perror("Socket failed");
        return RET_ERROR;
    }
    return RET_OK;
}

/**
 * @brief Configure socket options for address reuse.
 *
 * Allows reuse of local addresses and ports.
 *
 * @param server_fd Server socket file descriptor.
 * @param opt Pointer to socket options.
 * @param port Port number.
 * @return RET_OK on success, RET_ERROR on failure.
 */
static RetVal_t reuseTheAddress(int32_t server_fd, int32_t* opt, int16_t port){
    // Set socket options to reuse the address
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, opt, sizeof(*opt)) < 0) {
        perror("setsockopt failed");
        logMessageFormatted(LOG_LEVEL_ERROR, "TCPComm", "Setsockopt failed for port %d, with error: %s", 
                            port, strerror(errno));
        close(server_fd);
        return RET_ERROR;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, opt, sizeof(*opt)) < 0) {
        logMessageFormatted(LOG_LEVEL_ERROR, "TCPComm", "Setsockopt SO_REUSEPORT failed for port %d, with error: %s", 
                            port, strerror(errno));
        close(server_fd);
        return RET_ERROR;
    }

    return RET_OK;
}

/**
 * @brief Configure server address structure.
 *
 * Prepares the sockaddr_in structure for socket binding.
 *
 * @param server_addr Pointer to the sockaddr_in structure.
 * @param family Address family (e.g., AF_INET).
 * @param addr IP address to bind.
 * @param port Port number to bind.
 */
static void configureServerAddress(struct sockaddr_in* server_addr, 
                                   int32_t family, int32_t addr, int32_t port){
    server_addr->sin_family = family;
    server_addr->sin_addr.s_addr = addr; // Listen on any IP address
    server_addr->sin_port = htons(port); // Listening port
}

/**
 * @brief Bind the socket to a port and address.
 *
 * Associates the socket with a specific IP address and port.
 *
 * @param server_fd Server socket file descriptor.
 * @param server_addr Pointer to sockaddr structure.
 * @param size Size of sockaddr structure.
 * @param port Port number.
 * @return RET_OK on success, RET_ERROR on failure.
 */
static RetVal_t bindSocket(int32_t server_fd, struct sockaddr* server_addr, int32_t size, int16_t port){
    // Bind socket
    if (bind(server_fd, server_addr, size) < 0) {
        perror("Bind failed");
        logMessageFormatted(LOG_LEVEL_ERROR, "TCPComm", "Bind failed for port: %d, with error: %s", 
                            port, strerror(errno));
        close(server_fd);
        return RET_ERROR;
    }
    return RET_OK;
}

/**
 * @brief Start listening for incoming connections.
 *
 * Prepares the socket to accept client connections.
 *
 * @param server_fd Server socket file descriptor.
 * @param port Port number.
 * @return RET_OK on success, RET_ERROR on failure.
 */
static RetVal_t startListening(int32_t server_fd, int16_t port){
    // Start listening
    if (listen(server_fd, CONNECTION_REQUESTS) < 0) {
        logMessageFormatted(LOG_LEVEL_ERROR, "TCPComm", "Listen failed for port: %d, with error: %s", 
                            PORT, strerror(errno));
        close(server_fd);
        return RET_ERROR;
    }
    return RET_OK;
}

/**
 * @brief Accept an incoming client connection.
 *
 * Waits for a client to connect and accepts the connection.
 *
 * @param client_fd Pointer to client socket file descriptor.
 * @param server_fd Server socket file descriptor.
 * @param client_addr Pointer to client sockaddr structure.
 * @param client_len Pointer to client address length.
 * @return RET_OK on success, RET_ERROR on failure.
 */
static RetVal_t acceptClientConnection(int32_t* client_fd, int32_t server_fd, struct sockaddr_in* client_addr, socklen_t* client_len) {
    *client_fd = accept(server_fd, (struct sockaddr*)client_addr, client_len);
    if (*client_fd < 0) {
        // logMessageFormatted(LOG_LEVEL_ERROR, "TCPComm", "Accept failed with error: %s", strerror(errno));
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "TCPComm", "Client connected!");
    }
    return RET_OK;
}

/**
 * @brief Process client message.
 *
 * Parses and handles messages received from the client.
 *
 * @param buffer Pointer to the message buffer.
 * @return RET_OK on success, RET_ERROR on failure.
 */
static RetVal_t processClientMessage(const char* buffer) {
    int32_t id = 0;
    int32_t data = 0;

    if (sscanf(buffer, "ID=%d;DATA=%d", &id, &data) == TCP_MESSAGE_CLIENT_PARCED_INPUTS) {
        logMessageFormatted(LOG_LEVEL_DEBUG, "TCPComm", "Parsed ID: %d\n", id);
        logMessageFormatted(LOG_LEVEL_DEBUG, "TCPComm", "Parsed DATA: %d\n", data);
    } else {
        logMessageFormatted(LOG_LEVEL_DEBUG, "TCPComm", "Failed to parse buffer: %s\n", buffer);
    }
     
    if(handelStatus(data) != RET_OK){
        return RET_ERROR;
    }
    return RET_OK;
}

/**
 * @brief Handle communication with a connected client.
 *
 * Manages data exchange with the client.
 *
 * @param client_fd Client socket file descriptor.
 * @param buffer Pointer to message buffer.
 */
static void handleClientCommunication(int32_t client_fd, char* buffer) {
    ssize_t bytes_received;

    while (1) {
        do {
            bytes_received = recv(client_fd, buffer, TCP_BUFFER_SIZE, 0);
        } while (bytes_received < 0 && errno == EINTR);

        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            if(processClientMessage(buffer) != RET_OK){
                logMessage(LOG_LEVEL_ERROR, "TCPComm", "Failed to process client message");
            }
            send(client_fd, buffer, bytes_received, 0);
        } else if (bytes_received == 0) {
            logMessage(LOG_LEVEL_INFO, "TCPComm", "Client disconnected");
            break;
        } else {
            logMessageFormatted(LOG_LEVEL_ERROR, "TCPComm", "Recv failed with error: %s", strerror(errno));
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(TASTK_TIME_ECHO_SERVER_HANDLER)); 
    }
    close(client_fd);
    logMessage(LOG_LEVEL_INFO, "TCPComm", "Connection closed");
}

/**
 * @brief Entry point for the TCP Echo Server Task.
 */
void tcpEchoServerTask() {
    int32_t server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[TCP_BUFFER_SIZE] = {0};
    int opt = OPT_VALUE;

    if(createSocket(&server_fd, PORT) != RET_OK){
        vTaskDelete(NULL);
    }

    if(reuseTheAddress(server_fd, &opt, PORT) != RET_OK){
        vTaskDelete(NULL);
    }

    configureServerAddress(&server_addr, AF_INET, INADDR_ANY, PORT);

    if(bindSocket(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr), PORT) != RET_OK){
        vTaskDelete(NULL);
    }

    if(startListening(server_fd, PORT) != RET_OK){
        vTaskDelete(NULL);
    }
    
    while(1) {
        if(acceptClientConnection(&client_fd, server_fd, &client_addr, &client_len) != RET_OK){
            continue;
        }
        handleClientCommunication(client_fd, buffer);
    }

    close(server_fd);
    vTaskDelete(NULL);
}
