/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h> // Include pthread.h to use PTHREAD_STACK_MIN

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Include the task headers */
#include "master_handler.h"
#include "slave_handler.h"

/* Define the maximum number of messages and the maximum message size */
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE sizeof(QueueMessage)  // Size of a single message

/* Define the QueueMessage structure */
typedef struct {
    uint8_t id;
    uint16_t value;
} QueueMessage;

/* Queue handle */
QueueHandle_t xQueue; // Define xQueue here

int main(void)
{
    printf("Starting main function\n");

    /* Create the queue. It can hold MAX_MESSAGES number of QueueMessage structures */
    xQueue = xQueueCreate(MAX_MESSAGES, MAX_MSG_SIZE);

    if (xQueue == NULL) {
        printf("Failed to create queue\n");
        return 1;  // Return error code if queue creation fails
    } else {
        printf("Queue created successfully\n");
    }

    /* Create the master task */
    if (xTaskCreate(vMasterCommHandler, "MasterTask", PTHREAD_STACK_MIN, xQueue, 1, NULL) != pdPASS) {
        printf("Failed to create MasterTask\n");
        return 1;
    } else {
        printf("MasterTask created successfully\n");
    }

    /* Create the slave task */
    if (xTaskCreate(vSlaveCommHandler, "SlaveCommHandler", PTHREAD_STACK_MIN, xQueue, 1, NULL) != pdPASS) {
        printf("Failed to create SlaveTaskCommHandling\n");
        return 1;
    } else {
        printf("SlaveTaskCommHandling created successfully\n");
    }

    /* Create the slave task */
    if (xTaskCreate(vSlaveTaskTestHandling, "SlaveTaskTestHandling", PTHREAD_STACK_MIN, NULL, 1, NULL) != pdPASS) {
        printf("Failed to create SlaveTaskTestHandling\n");
        return 1;
    } else {
        printf("SlaveTaskTestHandling created successfully\n");
    }

    /* Start the FreeRTOS scheduler */
    printf("Starting FreeRTOS scheduler\n");
    vTaskStartScheduler();

    printf("Scheduler ended unexpectedly\n");
    return 0; // Should never reach here unless there's an error
}
