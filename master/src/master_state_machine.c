#include <stdio.h>
#include "master_state_machine.h"
#include "master_comm.h"
#include "types.h"
#include "logger.h"
#include "semphr.h"

/**
 * @file master_state_machine.c
 * @brief Implements the state machine for managing master states.
 *
 * This file defines the logic for managing and transitioning between different
 * operational states of a master system. The state transitions are thread-safe
 * and logged for clarity and debugging.
 */

/**
 * @brief Maps slave states to corresponding master states.
 *
 * This mapping ensures proper state transitions between slave and master systems.
 */
typedef struct {
    SlaveStates slaveState; ///< State of the slave system.
    MasterStates masterState; ///< Corresponding state in the master system.
} SlaveToMasterMap;

SlaveToMasterMap slaveToMasterMap[] = {
    {SLEEP, IDLE},
    {ACTIVE, PROCESSING},
    {FAULT, ERROR},
};

/**
 * @brief Structure for handling the master's state and synchronization.
 *
 * - currentState: Tracks the current state.
 * - stateSemaphore: Mutex for ensuring thread-safe state transitions.
 */
typedef struct {
    MasterStates currentState; ///< Current state of the master system.
    SemaphoreHandle_t stateSemaphore; ///< Semaphore for state synchronization.
} MasterStatesConditionHandler;

static MasterStatesConditionHandler masterStateMachineCondition = {IDLE, NULL};

// Forward declarations for state handler functions
static RetVal handleIdleState();
static RetVal handleProcessState();
static RetVal handleErrorState();

/**
 * @brief Master state machine mapping states to their handler functions.
 */
typedef struct {
    MasterStates state; ///< State identifier.
    RetVal (*handler)(void); ///< Pointer to the handler function for the state.
} MasterStateMachine;

/**
 * @brief Array mapping each state to its corresponding handler function.
 */
MasterStateMachine masterFSM[] = {
    {IDLE, handleIdleState},
    {PROCESSING, handleProcessState},
    {ERROR, handleErrorState},
};

/**
 * @brief Sets a new state for the master state machine.
 *
 * Ensures thread safety using a semaphore before transitioning to a new state.
 *
 * @param state The new state to transition to.
 * @return RET_OK on success, RET_ERROR on failure.
 */
static RetVal setNewState(MasterStates state) {
    RetVal ret = RET_OK;
    if (xSemaphoreTake(masterStateMachineCondition.stateSemaphore, (TickType_t)10) == pdTRUE) {
        if (state >= MAX_STATE_MASTER) {
            logMessage(LOG_LEVEL_ERROR, "MasterStateMachine", "Invalid state");
            ret = RET_ERROR;
        } else {
            if (masterStateMachineCondition.currentState != state) {
                masterStateMachineCondition.currentState = state;
                logMessageFormatted(LOG_LEVEL_INFO, "MasterStateMachine", "New status is %d", 
                                    masterStateMachineCondition.currentState);
            }
        }
        xSemaphoreGive(masterStateMachineCondition.stateSemaphore);
    } else {
        logMessage(LOG_LEVEL_ERROR, "MasterStateMachine", "Failed to take semaphore");
        ret = RET_ERROR;
    }
    return ret;
}

/**
 * @brief Handles the IDLE state logic.
 *
 * Transitions the master system into the IDLE state and logs the operation.
 *
 * @return RET_OK on success.
 */
static RetVal handleIdleState() {
    setNewState(IDLE);
    logMessage(LOG_LEVEL_INFO, "MasterStateMachine", "Master: Handling idle state");
    return RET_OK;
}

/**
 * @brief Handles the PROCESSING state logic.
 *
 * Transitions the master system into the PROCESSING state and logs the operation.
 *
 * @return RET_OK on success.
 */
static RetVal handleProcessState() {
    setNewState(PROCESSING);
    logMessage(LOG_LEVEL_INFO, "MasterStateMachine", "Master: Handling process state");
    return RET_OK;
}

/**
 * @brief Handles the ERROR state logic.
 *
 * Transitions the master system into the ERROR state and logs the operation.
 *
 * @return RET_OK on success.
 */
static RetVal handleErrorState() {
    setNewState(ERROR);
    logMessage(LOG_LEVEL_INFO, "MasterStateMachine", "Master: Handling error state");
    return RET_OK;
}

/**
 * @brief Initializes the semaphore used for state synchronization.
 *
 * Creates a binary semaphore to ensure thread safety during state transitions.
 *
 * @return RET_OK if initialization succeeded, RET_ERROR otherwise.
 */
RetVal initStateSemaphoreMaster() {
    masterStateMachineCondition.stateSemaphore = xSemaphoreCreateBinary();
    if (masterStateMachineCondition.stateSemaphore == NULL) {
        logMessage(LOG_LEVEL_ERROR, "MasterStateMachine", "Failed to create state semaphore");
        return RET_ERROR;
    }
    if (xSemaphoreGive(masterStateMachineCondition.stateSemaphore) != pdTRUE) {
        logMessage(LOG_LEVEL_ERROR, "MasterStateMachine", "Failed to give state semaphore");
        return RET_ERROR;
    }
    return RET_OK;
}

/**
 * @brief Dispatches the state to the appropriate handler.
 *
 * Maps slave states to master states and calls the respective handler.
 *
 * @param data The slave state to dispatch.
 * @return RET_OK on success, RET_ERROR otherwise.
 */
RetVal stateDispatcher(SlaveStates data) {
    RetVal ret = RET_OK;
    MasterStates state = slaveToMasterMap[data].masterState;
    if (state >= MAX_STATE_MASTER) {
        ret = RET_ERROR;
    }
    logMessageFormatted(LOG_LEVEL_DEBUG, "MasterStateMachine", "Dispatching state %d", data);

    if(state != masterStateMachineCondition.currentState){
        ret = masterFSM[state].handler();
    }

    return ret;
}

/**
 * @brief Retrieves the current state of the master.
 *
 * @param currentState Pointer to store the current state.
 * @return RET_OK on success.
 */
RetVal getCurrentState(MasterStates* currentState) {
    *currentState = masterStateMachineCondition.currentState;
    return RET_OK;
}
