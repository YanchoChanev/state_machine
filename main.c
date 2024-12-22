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
#include "slave_restart_threads.h"
#include "types.h"
#include "logger.h"

/* Define the maximum number of messages and the maximum message size */
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE sizeof(QueueMessage)  

// Queue handle 
static QueueHandle_t xQueue; 

static RetVal initComponents() {
    xQueue = xQueueCreate(MAX_MESSAGES, MAX_MSG_SIZE);

    printf("xQueue Address: %p\n", (void *)xQueue);

    if (xQueue == NULL) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create queue\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "Queue created successfully\n");
    }

    if(initStateSemaphoreMaster() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Init State Semaphore Master faild");
        return RET_ERROR;
    }

    if(initStateSemaphoreSlave() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Init State Semaphore Slave faild");
        return RET_ERROR;
    }

    initMasterComm(xQueue);
    initSlaveComm(xQueue);

    return RET_OK;
} 

static RetVal createStateQueue(){
    // Create the queue. It can hold MAX_MESSAGES number of QueueMessage structures
   

    return RET_OK;
}

static RetVal creatMasterTasks(){

    /* Create the master task */
    if (xTaskCreate(vMasterCommHandler, "MasterTask", PTHREAD_STACK_MIN, NULL, 
                    TASTK_PRIO_MASTER_COMM_HANDLER, NULL) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create MasterTask\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "MasterTask created successfully\n");
    }

    /* Create the master task */
    if (xTaskCreate(vMasterStatusCheckHandler, "MasterStatusCheckHandler", PTHREAD_STACK_MIN, NULL, 
                    TASTK_PRIO_MASTER_STATUS_CHECK_HANDLER, NULL) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create MasterTask\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "MasterTask created successfully\n");
    }
}

static RetVal creatSlaveTasks(){
    TaskHandle_t slaveTaskHandels[TAKS_HADLERS_SIZE] = {NULL, NULL, NULL, NULL};

    /* Create the slave task */
    if (xTaskCreate(vSlaveCommHandler, "SlaveCommHandler", PTHREAD_STACK_MIN, NULL, 
                    TASTK_PRIO_SLAVE_COMM_HANDLER, &slaveTaskHandels[COME_HANDLER_ID]) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create SlaveTaskCommHandling\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "SlaveTaskCommHandling created successfully\n");
    }

    /* Create the slave task */
    if (xTaskCreate(vSlaveTaskTestHandling, "SlaveTaskTestHandling", PTHREAD_STACK_MIN, NULL, 
                    TASTK_PRIO_SLAVE_TEST_TASK_HANDLER, &slaveTaskHandels[TASK_TEST_HANDLER_ID]) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create SlaveTaskTestHandling\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "SlaveTaskTestHandling created successfully\n");
    }

    /* Create the slave task */
    if (xTaskCreate(vSlaveStatusObservationHandler, "SlaveStatusObservationHandler", PTHREAD_STACK_MIN, NULL, 
                    TASTK_PRIO_SLAVE_STATUS_OBSERVATION_HANDLING, &slaveTaskHandels[SLAVE_STATUS_OBSERVATION_HANDLER_ID]) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create SlaveTaskTestHandling\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "SlaveTaskTestHandling created successfully\n");
    }

    /* Create the TCP Echo Server task */
    if (xTaskCreate(vTCPEchoServerTask, "TCPEchoServerTask", configMINIMAL_STACK_SIZE * 8, NULL, 
                    TASTK_PRIO_ECHO_SERVER_HANDLER, &slaveTaskHandels[TCP_ECHO_SERVER_TASK]) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create TCPEchoServerTask\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "TCPEchoServerTask created successfully\n");
    }

    /* Create the slave task */
    if (xTaskCreate(vRestartHandler, "RestartSlave", PTHREAD_STACK_MIN, NULL, 
                    TASTK_PRIO_SLAVE_RESTAT_STATUS, NULL) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create SlaveTaskTestHandling\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "SlaveTaskTestHandling created successfully\n");
    }

    setTaskHandlers(&slaveTaskHandels[0]);
    return RET_OK;
}

int main(void)
{
    if(createStateQueue() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Create State Queue faild");
        return 1;
    }
   
    if(initComponents() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Init Components faild");
        return 1;
    }

    if(creatMasterTasks() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Create Master Tasks faild");
        return 1;
    }

    if(creatSlaveTasks() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Create Slave Tasks faild");
        return 1;
    }

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();
    return 0; 
}
