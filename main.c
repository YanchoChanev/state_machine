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
static QueueHandle_t xQueue = NULL;
static QueueHandle_t resetQueueHandler = NULL;

static RetVal initComponents() {
    xQueue = xQueueCreate(MAX_MESSAGES, MAX_MSG_SIZE);
    resetQueueHandler = xQueueCreate(MAX_MESSAGES, sizeof(uint8_t));

    printf("xQueue Address: %p\n", (void *)xQueue);

    if (xQueue == NULL) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create queue\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "Queue created successfully\n");
    }

    printf("xQueue Address: %p\n", (void *)xQueue);
    if(initStateSemaphoreMaster() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Init State Semaphore Master faild");
        return RET_ERROR;
    }

    printf("xQueue Address: %p\n", (void *)xQueue);
    if(initStateMachineSlave() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Init State Semaphore Slave faild");
        return RET_ERROR;
    }

    printf("xQueue Address: %p\n", (void *)xQueue);
    initStateMachineSlave(resetQueueHandler);
    initMasterComm(xQueue);
    initSlaveComm(xQueue);

    return RET_OK;
} 

static RetVal createStateQueue(){
    // Create the queue. It can hold MAX_MESSAGES number of QueueMessage structures
   

    return RET_OK;
}

static RetVal creatMasterTasks(){

    printf("xQueue Address: %p\n", (void *)xQueue);
    /* Create the master task */
    if (xTaskCreate(vMasterCommHandler, "MasterTask", configMINIMAL_STACK_SIZE * 4, NULL, 
                    TASTK_PRIO_MASTER_COMM_HANDLER, NULL) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create MasterTask\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "MasterTask created successfully\n");
    }

    printf("xQueue Address: %p\n", (void *)xQueue);
    /* Create the master task */
    if (xTaskCreate(vMasterStatusCheckHandler, "MasterStatusCheckHandler", configMINIMAL_STACK_SIZE * 4, NULL, 
                    TASTK_PRIO_MASTER_STATUS_CHECK_HANDLER, NULL) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create MasterTask\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "MasterTask created successfully\n");
    }
}

static RetVal creatSlaveTasks(){
    TaskHandle_t slaveTaskHandels[TAKS_HADLERS_SIZE] = {NULL, NULL};

    printf("SlaveStatusObservationHandler\n");
    /* Create the slave task */
    if (xTaskCreate(vSlaveStatusObservationHandler, "SlaveStatusObservationHandler", configMINIMAL_STACK_SIZE * 4, resetQueueHandler, 
                    TASTK_PRIO_SLAVE_STATUS_OBSERVATION_HANDLING, &slaveTaskHandels[SLAVE_STATUS_OBSERVATION_HANDLER_ID]) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create vSlaveStatusObservationHandler\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "vSlaveStatusObservationHandler created successfully\n");
    }

    printf("TCPEchoServerTask\n");
    /* Create the slave task */
    if (xTaskCreate(vTCPEchoServerTask, "TCPEchoServerTask", configMINIMAL_STACK_SIZE * 16, NULL, 
                    TASTK_PRIO_ECHO_SERVER_HANDLER, &slaveTaskHandels[TCP_ECHO_SERVER_TASK]) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create vTCPEchoServerTask\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "vTCPEchoServerTask created successfully\n");
    }

    printf("RestartSlave\n");
    /* Create the slave task */
    if (xTaskCreate(vRestartHandler, "RestartSlave", configMINIMAL_STACK_SIZE * 16, resetQueueHandler, 
                    TASTK_PRIO_SLAVE_RESTAT_STATUS, NULL) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create vRestartHandler\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "vRestartHandler created successfully\n");
    }

    printf("TaskHandler\n");
    setTaskHandlers(&slaveTaskHandels[0]);
    return RET_OK;
}

int main(void)
{
    printf("State Synchronization Example\n");
    if(createStateQueue() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Create State Queue faild");
        return 1;
    }
   
    printf("Init Components\n");
    if(initComponents() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Init Components faild");
        return 1;
    }

    printf("Create Slave Tasks\n");
    if(creatSlaveTasks() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Create Slave Tasks faild");
        return 1;
    }

    printf("Create Master Tasks\n");
    if(creatMasterTasks() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Create Master Tasks faild");
        return 1;
    }

    printf("Start the FreeRTOS scheduler\n");
    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();
    return 0; 
}
