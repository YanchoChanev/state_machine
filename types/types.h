#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef enum {
    FALSE,
    TRUE
} Bool;

/**
 * @brief Structure defining a queue message.
 */
typedef struct {
    uint8_t msg_id; ///< Message identifier.
    uint8_t state;  ///< State associated with the message.
} QueueMessage;

/**
 * @brief Enumeration of message types.
 */
typedef enum {
    HANDEL_SLAVE_FAULT,  ///< Handle slave fault.
    RESET_SLAVE,         ///< Reset slave.
    SLAVE_CURRENT_STATUS ///< Current status of the slave.
} MessageType;

/**
 * @brief Enumeration of master states.
 */
typedef enum {
    IDLE,            ///< Idle state.
    PROCESSING,      ///< Processing state.
    ERROR,           ///< Error state.
    MAX_STATE_MASTER ///< Maximum state value for master.
} MasterStates;

/**
 * @brief Enumeration of slave states.
 */
typedef enum {
    SLEEP,          ///< Sleep state.
    ACTIVE,         ///< Active state.
    FAULT,          ///< Fault state.
    RESET,          ///< Reset state.
    MAX_STATE_SLAVE ///< Maximum state value for slave.
} SlaveStates;

typedef struct {
    SlaveStates status;
    Bool isNew;
} SlaveStatesCondition;

typedef struct {
    MasterStates status;
    Bool isNew;
} MasterStatesCondition;

/**
 * @brief Enumeration of return values.
 */
typedef enum {
    RET_OK,    ///< Operation succeeded.
    RET_ERROR  ///< Operation failed.
} RetVal;

// Task prioritis
#define TASTK_PRIO_MASTER_COMM_HANDLER               1
#define TASTK_PRIO_MASTER_STATUS_CHECK_HANDLER       1
#define TASTK_PRIO_SLAVE_STATUS_OBSERVATION_HANDLING 1
#define TASTK_PRIO_SLAVE_RESTAT_STATUS               2
#define TASTK_PRIO_ECHO_SERVER_HANDLER               1

#define TASTK_TIME_MASTER_COMM_HANDLER               10
#define TASTK_TIME_MASTER_STATUS_CHECK_HANDLER       500
#define TASTK_TIME_SLAVE_STATUS_OBSERVATION_HANDLING 80
#define TASTK_TIME_SLAVE_RESTAT_STATUS               10
#define TASTK_TIME_ECHO_SERVER_HANDLER               80


#endif // TYPES_H