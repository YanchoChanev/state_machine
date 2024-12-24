#ifndef SLAVE_STATE_MACHINE_H
#define SLAVE_STATE_MACHINE_H

#include "types.h"
#include "FreeRTOS.h"
#include "queue.h"

/**
 * @file slave_state_machine.h
 * @brief Header file for the Slave State Machine module.
 *
 * This file contains the declarations for managing the state machine
 * of a slave system. It includes functions for initializing the state machine,
 * handling state transitions, and retrieving the current state.
 */

/**
 * @brief Initializes the semaphore used for state synchronization.
 *
 * This function sets up the necessary synchronization primitives, such as semaphores,
 * to ensure safe and thread-safe state transitions within the slave system.
 *
 * @param resetHandler Queue handle for handling reset state transitions.
 * @return RET_OK if initialization was successful, RET_ERROR otherwise.
 */
RetVal initStateMachineSlave(QueueHandle_t resetHandler);

/**
 * @brief Handles a change in the slave's status/state.
 *
 * This function processes a requested state transition and ensures that
 * the slave transitions to the specified state safely.
 *
 * @param state The new state to transition to.
 * @return RET_OK if the state change was successful, RET_ERROR otherwise.
 */
RetVal handelStatus(SlaveStates state);

/**
 * @brief Retrieves the current state of the slave.
 *
 * This function fetches the current operational state of the slave system
 * and stores it in the provided pointer.
 *
 * @param currentStatus Pointer to store the current state of the slave.
 * @return RET_OK if the current state was successfully retrieved, RET_ERROR otherwise.
 */
RetVal getState(SlaveStates* currentStatus);

#endif // SLAVE_STATE_MACHINE_H