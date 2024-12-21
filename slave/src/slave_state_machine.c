#include <stdio.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "logger.h"
#include "slave_comm.h"
#include "slave_state_machine.h"
#include "types.h"

// Queue handle for state signals.
static QueueHandle_t stateQueueHandle_ = NULL;

// Current state of the slave.
static SlaveStates currentState_ = SLEEP;

// Semaphore for state synchronization.
static SemaphoreHandle_t statusSemaphore = NULL;

// Type definition for state handler functions.
typedef void (*StateHandler)();

// Function prototypes for state handlers.
static void handleSleepState();
static void handleActiveState();
static void handleFaultState();

// Function prototype for changing state.
static RetVal changeState(SlaveStates state);

#define SLAVE_MAX_STATE_SLAVE (MAX_STATE_SLAVE + 1)

// State handler function array.
static StateHandler state_handlers[SLAVE_MAX_STATE_SLAVE] = {
    handleSleepState,
    handleActiveState,
    handleFaultState,
    NULL
};

/**
 * @brief Changes the current state of the slave.
 * @param state The new state to transition to.
 * @return RET_OK on success, RET_ERROR on failure.
 */
static RetVal changeState(SlaveStates state) {
    if (statusSemaphore == NULL) {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "statusSemaphore is NULL");
        return RET_ERROR;
    }

    if (xSemaphoreTake(statusSemaphore, portMAX_DELAY) == pdTRUE) {
        if (currentState_ != state) {
            currentState_ = state;
            logMessageFormatted(LOG_LEVEL_INFO, "SlaveStateMachine", "New state is %d", currentState_);
        }
        xSemaphoreGive(statusSemaphore);
        return RET_OK;
    } else {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "Failed to take status semaphore");
        return RET_ERROR;
    }
}

/**
 * @brief Handles the FAULT state logic.
 * @return RET_OK on success, RET_ERROR on failure.
 */
static void handleFaultState() {
    logMessage(LOG_LEVEL_INFO, "SlaveStateMachine", "Slave: Handling FAULT state");
    QueueMessage data = {HANDEL_SLAVE_FAULT, ERROR};

    if (sendMsgSlave(STATE_CHANNEL, &data) != RET_OK) {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "Problem sending message over the queue");
        // return RET_ERROR;
    }
    // return RET_OK;
}

/**
 * @brief Handles the SLEEP state logic.
 * @return RET_OK on success.
 */
static void handleSleepState() {
    logMessage(LOG_LEVEL_INFO, "SlaveStateMachine", "Slave: Handling SLEEP state");
    // return RET_OK;
}

/**
 * @brief Handles the ACTIVE state logic.
 * @return RET_OK on success.
 */
static void handleActiveState() {
    logMessage(LOG_LEVEL_INFO, "SlaveStateMachine", "Slave: Handling ACTIVE state");
    // return RET_OK;
}

// Initializes the semaphore used for state synchronization.
RetVal initStateSemaphoreSlave() {
    statusSemaphore = xSemaphoreCreateMutex();
    if (statusSemaphore == NULL) {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "Failed to create status semaphore");
        return RET_ERROR;
    }
    return RET_OK;
}

// Handles a change in the slave's status/state.
RetVal handelStatus(SlaveStates state) {
    if (state >= SLAVE_MAX_STATE_SLAVE) {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "Invalid state");
        return RET_ERROR;
    }
    printf("State: %d\n", state);
    if (changeState(state) == RET_ERROR) {
        logMessage(LOG_LEVEL_ERROR, "SlaveStateMachine", "State change failed");
        return RET_ERROR;

        state_handlers[state]();
        // if (state_handlers[state]() == RET_ERROR) {
        //     logMessageFormatted(LOG_LEVEL_ERROR, "SlaveStateMachine", "Problem with state handler for state: %d", state);
        //     return RET_ERROR;
        // }
    }

    return RET_OK;
}

// Retrieves the current state of the slave.
SlaveStates getState() {
    return currentState_;
}