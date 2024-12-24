#include <stdio.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "logger.h"
#include "slave_comm.h"
#include "slave_state_machine.h"
#include "slave_restart_threads.h"
#include "types.h"
#include "state_mashine_types.h"

/**
 * @file slave_state_machine.c
 * @brief Implements the state machine for managing slave states.
 *
 * This file provides functionality for managing different operational states
 * of a slave system, ensuring state transitions are thread-safe and logged appropriately.
 */

/**
 * @brief Maximum valid state value for the slave.
 */
#define SLAVE_MAX_STATE_SLAVE (SLAVE_STATE_MAX + 1)

/**
 * @brief Maps slave states to corresponding master states.
 *
 * This mapping ensures proper state transitions between slave and master systems.
 */
typedef struct {
    MasterStates masterState; ///< Corresponding state in the master system.
    SlaveStates slaveState; ///< State of the slave system.
} MasterToSlaveMap;

MasterToSlaveMap masterToSlaveMap[] = {
    {SLAVE_INPUT_STATE_IDEL_OR_SLEEP,    SLAVE_STATE_SLEEP},
    {SLAVE_INPUT_STATE_RPOCES_OR_ACTIVE, SLAVE_STATE_ACTIVE},
    {SLAVE_INPUT_STATE_ERROR_OR_FAULT,   SLAVE_STATE_FAULT},
    {SLAVE_INPUT_STATE_ERROR_OR_RESET,   SLAVE_STATE_RESET},
};

/**
 * @brief StateHandler structure holds the state management data for the slave.
 *
 * - resetQueueHandler: Handle to the reset queue for communication.
 * - currentState: Tracks the current state of the slave.
 * - statusSemaphore: Mutex for ensuring safe access to the current state.
 */
typedef struct 
{
    QueueHandle_t resetQueueHandler;
    SlaveStates currentState;
    SemaphoreHandle_t statusSemaphore;
} StateHandler;

/**
 * @brief Global instance of StateHandler initialized to default values.
 */
static StateHandler stateHandler = {NULL, SLAVE_STATE_SLEEP, NULL};

// Forward declarations for state handler functions
static RetVal_t handleSleepState();
static RetVal_t handleActiveState();
static RetVal_t handleFaultState();
static RetVal_t handleResetState();

/**
 * @brief Master state machine mapping states to their handler functions.
 */
typedef struct {
    MasterStates state;
    RetVal_t (*handler)(void);
} SlaveStateMachine;

/**
 * @brief Array that maps each state to its corresponding handler function.
 */
SlaveStateMachine slaveFSM[] = {
    {SLAVE_STATE_SLEEP,  handleSleepState},
    {SLAVE_STATE_ACTIVE, handleActiveState},
    {SLAVE_STATE_FAULT,  handleFaultState},
    {SLAVE_STATE_RESET,  handleResetState}
};

/**
 * @brief Changes the current state of the slave.
 *
 * Ensures thread safety using a semaphore while performing state transitions.
 *
 * @param state New state to transition to.
 * @return RET_OK if state change was successful, RET_ERROR otherwise.
 */
static RetVal_t changeState(SlaveStates state) {
    if (stateHandler.statusSemaphore == NULL) {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "statusSemaphore is NULL");
        return RET_ERROR;
    }

    // Lock semaphore to ensure thread-safe state change
    if (xSemaphoreTake(stateHandler.statusSemaphore, portMAX_DELAY) == pdTRUE) {
        if (stateHandler.currentState != state) {
            stateHandler.currentState = state;
            logMessageFormatted(LOG_LEVEL_INFO, "SlaveStateMachine", "New state is %d", stateHandler.currentState);
        }
        xSemaphoreGive(stateHandler.statusSemaphore);
        return RET_OK;
    } else {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "Failed to take status semaphore");
        return RET_ERROR;
    }
}

/**
 * @brief Handles the FAULT state of the slave.
 *
 * @return RET_OK if state was successfully handled, RET_ERROR otherwise.
 */
static RetVal_t handleFaultState() {
    logMessage(LOG_LEVEL_INFO, "SlaveStateMachine", "Slave: Handling FAULT state");
    if(changeState(SLAVE_STATE_FAULT) != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "Failed to change state to FAULT");
        return RET_ERROR;
    }
    return RET_OK;
}

/**
 * @brief Handles the SLEEP state of the slave.
 *
 * @return RET_OK if state was successfully handled, RET_ERROR otherwise.
 */
static RetVal_t handleSleepState() {
    logMessage(LOG_LEVEL_INFO, "SlaveStateMachine", "Slave: Handling SLEEP state");
    if(changeState(SLAVE_STATE_SLEEP) != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "Failed to change state to SLEEP");
        return RET_ERROR;
    }
    return RET_OK;
}

/**
 * @brief Handles the ACTIVE state of the slave.
 *
 * @return RET_OK if state was successfully handled, RET_ERROR otherwise.
 */
static RetVal_t handleActiveState() {
    logMessage(LOG_LEVEL_INFO, "SlaveStateMachine", "Slave: Handling ACTIVE state");
    if(changeState(SLAVE_STATE_ACTIVE) != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "Failed to change state to ACTIVE");
        return RET_ERROR;
    }
    return RET_OK;
}

/**
 * @brief Handles the RESET state of the slave.
 *
 * @return RET_OK if state was successfully handled, RET_ERROR otherwise.
 */
static RetVal_t handleResetState() {
    int8_t signal = 1;
    logMessage(LOG_LEVEL_INFO, "SlaveStateMachine", "Slave: Handling RESET state");
    if(changeState(SLAVE_STATE_SLEEP) != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "Failed to change state to SLEEP");
        return RET_ERROR;
    }

    if(stateHandler.resetQueueHandler == NULL){
        return RET_ERROR;
    }

    if (xQueueSend(stateHandler.resetQueueHandler, &signal, portMAX_DELAY) != pdPASS) {
        logMessage(LOG_LEVEL_ERROR, "SlaveHandler", "Failed to send reset signal");
    }
    return RET_OK;
}

/**
 * @brief Initializes the semaphore used for managing state transitions.
 *
 * @return RET_OK if initialization succeeded, RET_ERROR otherwise.
 */
RetVal_t initStateMachineSlave(QueueHandle_t resetHandler) {
    stateHandler.resetQueueHandler = resetHandler;
    stateHandler.statusSemaphore = xSemaphoreCreateMutex();
    if (stateHandler.statusSemaphore == NULL) {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "Failed to create status semaphore");
        return RET_ERROR;
    }
    return RET_OK;
}

/**
 * @brief Handles the given state by calling the appropriate handler function.
 *
 * @param state State to handle.
 * @return RET_OK if state was successfully handled, RET_ERROR otherwise.
 */
RetVal_t handelStatus(SlaveStates state) {
    if (state >= SLAVE_MAX_STATE_SLAVE) {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "Invalid state");
        return RET_ERROR;
    }
    return slaveFSM[state].handler();
}

/**
 * @brief Retrieves the current state of the slave.
 *
 * @param currentStatus Pointer to store the current state.
 * @return RET_OK if successful, RET_ERROR otherwise.
 */
RetVal_t getState(SlaveStates* currentStatus) {
    if (currentStatus == NULL) {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "currentStatus is NULL");
        return RET_ERROR;
    }
    *currentStatus = stateHandler.currentState;
    return RET_OK;
}
