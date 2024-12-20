#include "slave_handler.h"
#include "slave_comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "FreeRTOS.h"
#include "task.h"
#include "types.h"

void vSlaveTaskCommHandling(void* args){
    printf("vSlaveTaskCommHandling created\n");
    initComunicationSlave(args);
    // MasterMessageRes data;
    
    // while(1){
    //     reciveMsgSlave(&data);
    //     printf("Message ID = %d, message state = %d\n", data.msg_id, data.state);
    // }

    vTaskDelay(pdMS_TO_TICKS(10));
}

void vSlaveTaskTestHandling(void* args){
    char input[16]; // Buffer for input
    int number;

    printf("vSlaveTaskTestHandling created\n");
    printf("Enter a number between 1 and 5: ");
    while(1){
        
        fflush(stdout); // Ensure the prompt is displayed immediately

        // Read input from the console
        if (fgets(input, sizeof(input), stdin) != NULL) {
            // Convert input to an integer
            number = atoi(input);

            // Validate the number
            if (number >= 0 && number <= 5) {
                printf("You entered a valid number: %d\n", number);
                statusChange(number);
                // Process the number (replace this with your logic)
            } else {
                printf("Invalid input. Please enter a number between 1 and 5.\n");
            }
        } else {
            // printf("Error reading input.\n");
        }

        // // Simulate task delay to prevent tight looping
        // vTaskDelay(pdMS_TO_TICKS(500));
    }

    vTaskDelay(pdMS_TO_TICKS(100));
}

void vSlaveCommHandler(void *args){
    printf("Slave is created\n");
    QueueHandle_t myQueueHandle = (QueueHandle_t)args;
    initComunicationSlave(myQueueHandle);

    QueueMessage data;
    
    while(1){
        reciveMsgSlave(&data);
        printf("Message ID = %d, message state = %d\n", data.msg_id, data.state);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}