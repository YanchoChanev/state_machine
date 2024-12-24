#ifndef MASTER_STATE_MACHINE_H
#define MASTER_STATE_MACHINE_H

#include "types.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "state_mashine_types.h"

/**
 * @file master_state_machine.h
 * @brief Header file for the Master State Machine module.
 *
 * This file defines the public interface for managing the master state machine,
 * including initialization, state dispatching, and state retrieval.
 */

/**
 * @brief Initializes the state semaphore for master state synchronization.
 *
 * This function sets up the semaphore required for thread-safe state transitions
 * in the master state machine.
 *
 * @return RET_OK if the semaphore was successfully initialized, RET_ERROR otherwise.
 */
RetVal_t initStateSemaphoreMaster();

/**
 * @brief Dispatches states to appropriate state handlers.
 *
 * This function processes incoming slave states and maps them to their respective
 * handler functions within the master state machine.
 *
 * @param data The state received from the slave.
 * @return RET_OK if the state was successfully dispatched, RET_ERROR otherwise.
 */
RetVal_t stateDispatcher(SlaveStates data);

/**
 * @brief Retrieves the current state of the master.
 *
 * This function fetches the current state of the master and stores it in the
 * provided pointer.
 *
 * @param currentState Pointer to store the current master state.
 * @return RET_OK if the current state was successfully retrieved, RET_ERROR otherwise.
 */
RetVal_t getCurrentState(MasterStates* currentState);

#endif // MASTER_STATE_MACHINE_H
