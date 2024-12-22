#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "FreeRTOS.h"
#include "task.h"
#include "TCP_comm.h"
#include "logger.h"

#define ipconfigUSE_DHCP        1
#define ipconfigIP_ADDRESS      "192.168.1.100"
#define ipconfigNET_MASK        "255.255.255.0"
#define ipconfigGATEWAY_ADDRESS "192.168.1.1"
#define ipconfigDNS_SERVER_ADDRESS "8.8.8.8"

// Example implementation of tcpEchoServerTask
void tcpEchoServerTask() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024] = {0};

    logMessage(LOG_LEVEL_INFO, "TCPComm", "tcpEchoServerTask started");

    // Create a socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket failed");
        logMessage(LOG_LEVEL_ERROR, "TCPComm", "Socket creation failed");
        vTaskDelete(NULL);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on any IP address
    server_addr.sin_port = htons(5001); // Listening port

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        logMessage(LOG_LEVEL_ERROR, "TCPComm", "Bind failed");
        close(server_fd);
        vTaskDelete(NULL);
    }

    // Start listening
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        logMessage(LOG_LEVEL_ERROR, "TCPComm", "Listen failed");
        close(server_fd);
        vTaskDelete(NULL);
    }

    logMessage(LOG_LEVEL_INFO, "TCPComm", "Server listening on port 5001...");

    for (;;) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("Accept failed");
            logMessage(LOG_LEVEL_ERROR, "TCPComm", "Accept failed");
            continue;
        }

        logMessage(LOG_LEVEL_INFO, "TCPComm", "Client connected!");

        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            logMessageFormatted(LOG_LEVEL_INFO, "TCPComm", "Received: %s", buffer);

            // Echo back to client
            send(client_fd, buffer, bytes_received, 0);
        }

        close(client_fd);
    }

    close(server_fd);
    logMessage(LOG_LEVEL_INFO, "TCPComm", "tcpEchoServerTask completed");
    vTaskDelete(NULL);
}
