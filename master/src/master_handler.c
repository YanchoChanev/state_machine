#include "master_handler.h"
#include "master_comm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "types.h"

void vMasterCommHandler(void *args){
    printf("Master is created\n");
    QueueHandle_t myQueueHandle = (QueueHandle_t)args;
    initCommunicationMaster(myQueueHandle);

    QueueMessage data;
    
    while(1){
        reciveMsgMaster(&data);
        printf("Message ID = %d, message state = %d\n", data.msg_id, data.state);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}