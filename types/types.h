#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

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
    MAX_STATE_SLAVE ///< Maximum state value for slave.
} SlaveStates;

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
#define TASTK_PRIO_SLAVE_COMM_HANDLER                2 
#define TASTK_PRIO_SLAVE_TEST_TASK_HANDLER           1 
#define TASTK_PRIO_SLAVE_STATUS_OBSERVATION_HANDLING 1
#define TASTK_PRIO_SLAVE_RESTAT_STATUS               3
#define TASTK_PRIO_ECHO_SERVER_HANDLER               2
#endif // TYPES_H