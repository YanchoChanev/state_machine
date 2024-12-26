#ifndef STATE_MACHINE_TYPES_H
#define STATE_MACHINE_TYPES_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file state_machine_types.h
 * @brief State definitions for Master and Slave systems.
 *
 * This file defines enumerations for the states used in both master and slave
 * state machines, as well as specific input states for slave transitions.
 */

/**
 * @brief Enumeration of master states.
 *
 * Defines the possible states for the master system.
 */
typedef enum {
    MASTESR_STATE_IDLE,            ///< Idle state.
    MASTESR_STATE_PROCESSING,      ///< Processing state.
    MASTESR_STATE_ERROR,           ///< Error state.
    MASTESR_STATE_MAX              ///< Maximum state value for master.
} MasterStates;

/**
 * @brief Enumeration of slave states.
 *
 * Defines the possible states for the slave system.
 */
typedef enum {
    SLAVE_STATE_SLEEP,          ///< Sleep state.
    SLAVE_STATE_ACTIVE,         ///< Active state.
    SLAVE_STATE_FAULT,          ///< Fault state.
    SLAVE_STATE_RESET,          ///< Reset state.
    SLAVE_STATE_MAX             ///< Maximum state value for slave.
} SlaveStates;

/**
 * @brief Enumeration of slave input states.
 *
 * Defines possible input states used for transitions and validations in the slave system.
 */
typedef enum {
    SLAVE_INPUT_STATE_IDEL_OR_SLEEP,    ///< Idle or Sleep state.
    SLAVE_INPUT_STATE_RPOCES_OR_ACTIVE, ///< Processing or Active state.
    SLAVE_INPUT_STATE_ERROR_OR_FAULT,   ///< Error or Fault state.
    SLAVE_INPUT_STATE_ERROR_OR_RESET,   ///< Error or Reset state.
    SLAVE_INPUT_STATE_MAX               ///< Maximum input state value.
} SlaveInputStates;

#ifdef __cplusplus
}
#endif

#endif // STATE_MACHINE_TYPES_H
