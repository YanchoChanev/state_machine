#include <stdio.h>
#include "master_state_machine.h"
#include "master_comm.h"
#include "types.h"
#include "logger.h"
#include "semphr.h"

// Queue handle for state signals.
static QueueHandle_t stateQueueHandle_ = NULL;

// Current state of the master.
static MasterStates currentState_ = IDLE;

// Semaphore for state synchronization.
static SemaphoreHandle_t stateSemaphore_;

// Function pointer type for state handler functions.
typedef RetVal (*StateHandler)();

// Function prototypes for state handlers.
static RetVal handleIdleState();
static RetVal handleProcessState();
static RetVal handleErrorState();

// State handler function array.
static StateHandler stateHandlers_[MAX_STATE_MASTER] = {
    handleIdleState,
    handleProcessState,
    handleErrorState,
    NULL
};

/**
 * @brief Internal function for handles the IDLE state logic.
 * @return RET_OK on success.
 */
static RetVal handleIdleState() {
    logMessage(LOG_LEVEL_INFO, "MasterStateMachine", "Master: Handling idle state");
    return RET_OK;
}

/**
 * @brief Internal function for handles the PROCESS state logic.
 * @return RET_OK on success.
 */
static RetVal handleProcessState() {
    logMessage(LOG_LEVEL_INFO, "MasterStateMachine", "Master: Handling process state");
    return RET_OK;
}

/**
 * @brief Internal function for handles the ERROR state logic.
 * @return RET_OK on success, RET_ERROR on failure.
 */
static RetVal handleErrorState() {

    RetVal ret = RET_OK;
    QueueMessage data = {RESET_SLAVE, ERROR};
    if (sendMsgMaster(&data) != RET_OK) {
        logMessage(LOG_LEVEL_ERROR, "MasterStateMachine", "Send message to slave failed");
        ret = RET_ERROR;
    }
    return ret;
}

/**
 * @brief Internal function for sets a new state for the master state machine.
 * @param state The new state to transition to.
 * @return RET_OK on success, RET_ERROR on failure.
 */
static RetVal setNewState(MasterStates state) {
    RetVal ret = RET_OK;
    if (xSemaphoreTake(stateSemaphore_, (TickType_t)10) == pdTRUE) {
        if (state >= MAX_STATE_MASTER) {
            logMessage(LOG_LEVEL_ERROR, "MasterStateMachine", "Invalid state");
            ret = RET_ERROR;
        } else {
            if (currentState_ != state) {
                currentState_ = state;
                logMessageFormatted(LOG_LEVEL_INFO, "MasterStateMachine", "New status is %d", currentState_);
                stateHandlers_[state]();
            }
        }
        xSemaphoreGive(stateSemaphore_);
    } else {
        logMessage(LOG_LEVEL_ERROR, "MasterStateMachine", "Failed to take semaphore");
        ret = RET_ERROR;
    }
    return ret;
}

// Initializes the state semaphore for master state synchronization.
RetVal initStateSemaphoreMaster() {
    stateSemaphore_ = xSemaphoreCreateBinary();
    if (stateSemaphore_ == NULL) {
        logMessage(LOG_LEVEL_ERROR, "MasterStateMachine", "Failed to create state semaphore");
        return RET_ERROR;
    }
    if (xSemaphoreGive(stateSemaphore_) != pdTRUE) {
        logMessage(LOG_LEVEL_ERROR, "MasterStateMachine", "Failed to give state semaphore");
        return RET_ERROR;
    }
    return RET_OK;
}

// Dispatches states to appropriate state handlers.
RetVal stateDispatcher(QueueMessage data) {
    // printf("Message ID = %d, message state = %d\n", data.msg_id, data.state);
    // RetVal ret = RET_OK;
    // if (data.msg_id == HANDEL_SLAVE_FAULT) {
    //     if (data.state == FAULT) {
    //         setNewState(ERROR);
    //     }
    // } else {
    //     switch (data.state) {
    //         case ACTIVE:
    //             if (setNewState(PROCESSING) != RET_OK) {
    //                 logMessage(LOG_LEVEL_ERROR, "MasterStateMachine", "Failed to set state PROCESSING");
    //                 ret = RET_ERROR;
    //             }
    //             break;
    //         case SLEEP:
    //             if (setNewState(IDLE) != RET_OK) {
    //                 logMessage(LOG_LEVEL_ERROR, "MasterStateMachine", "Failed to set state IDLE");
    //                 ret = RET_ERROR;
    //             }
    //             break;
    //     }
    // }

    switch(data.state)
    {
        case ACTIVE:
            if(setNewState(PROCESSING) != RET_OK)
            {
                logMessage(LOG_LEVEL_ERROR, "MasterStateMachine", "Failed to set state PROCESSING");
                return RET_ERROR;
            }
            break;
        case SLEEP:
            if(setNewState(IDLE) != RET_OK)
            {
                logMessage(LOG_LEVEL_ERROR, "MasterStateMachine", "Failed to set state IDLE");
                return RET_ERROR;
            }
            break;
        case FAULT:
            if(setNewState(ERROR) != RET_OK)
            {
                logMessage(LOG_LEVEL_ERROR, "MasterStateMachine", "Failed to set state ERROR");
                return RET_ERROR;
            }
            break;
        default:
            logMessage(LOG_LEVEL_ERROR, "MasterStateMachine", "Invalid state");
            return RET_ERROR;
    }
    // return ret;
}

// Retrieves the current state of the master.
MasterStates getCurrentState() {
    return currentState_;
}

// Sets the state queue handler.
RetVal setStateQueueHandlerMaster(QueueHandle_t stateQueueHandle) {
    if (stateQueueHandle != NULL) {
        stateQueueHandle_ = stateQueueHandle;
        return RET_OK;
    } else {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "stateQueueHandle is NULL");
        return RET_ERROR;
    }
}

// Retrieves the state queue handler.
RetVal getStateQueueHandlerMaster(QueueHandle_t stateQueueHandle) {
    if (stateQueueHandle_ == NULL) {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "stateQueueHandle is NULL");
        return RET_ERROR;
    }
    stateQueueHandle = stateQueueHandle_;
    return RET_OK;
}