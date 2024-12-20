#include "master_state_machine.h"

static MasteStates current_state = IDEL;

void setNewState(MasteStates state){
    if(state >= MAX_STATE_MASTER){
        logMessage("ERROR", "MasterStateMachine", "Invalid state");
        return;
    } 
    else{
        current_state = state;
        printf("New Status is %d\n", current_state);
        char message[50];
        snprintf(message, sizeof(message), "New status is %d\n", current_state);
        logMessage("DEBUG", "MasterStateMachine", message);
    }
}

MasteStates getCurrentState(){
    return current_state;
}