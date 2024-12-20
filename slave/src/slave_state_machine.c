#include "slave_state_machine.h"
#include "slave_comm.h"
#include "types.h"
#include "logger.h"
#include <stdio.h>

typedef void (*StateHandler)();

static void handleSleepState();
static void handleActiveState();
static void handleFaultState();

static StateHandler state_handlers[4] = {
    handleSleepState,
    handleActiveState,
    handleFaultState,
    NULL
};

static current_state = SLEEP;

void statusChange(SlaveStates state){
    if(state >= MAX_STATE_SLAVE){
        logMessage("ERROR", "SlaveStateMachine", "Invalid state");
        return;
    }
    else{
        current_state = state;
        printf("New Status is %d\n", current_state);
        char message[50];
        snprintf(message, sizeof(message), "New status is %d\n", current_state);
        logMessage("DEBUG", "SlaveStateMachine", message);
    }
    
    // if(state_handlers[state] != NULL){
        state_handlers[state]();
    // }
}

SlaveStates getState(){
    return current_state;
}

static void handleFaultState(){
    printf("Slave: Handling fault state\n");
    QueueMessage data = {HANDEL_SLAVE_FAULT, ERROR};
    sendMsgSlave(&data);
}

// Define other state handlers if needed
static void handleSleepState() {
    // Implementation for sleep state
}

static void handleActiveState() {
    // Implementation for active state
}