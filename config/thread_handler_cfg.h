#ifndef THREAD_HANDLER_CFG_H
#define THREAD_HANDLER_CFG_H

/**
 * @file thread_handler_cfg.h
 * @brief Configuration file for thread and task handler priorities and delays.
 *
 * This file defines macros for task handler counts, priorities, and timing
 * configurations for both master and slave systems.
 */

/**
 * @brief Number of task handlers defined.
 *
 * This macro defines the total number of task handlers that can be managed
 * by the slave restart system.
 */
#define SLAVE_TAKS_HANDLERS_SIZE 2

/**
 * @brief Delay before restarting tasks (in milliseconds).
 *
 * Defines the waiting period before attempting to restart tasks.
 */
#define DELAY_BEFORE_RESTART 3000

/**
 * @brief Task priorities for various handlers.
 *
 * These macros define the priority levels assigned to different tasks
 * for both master and slave systems.
 */
#define TASTK_PRIO_MASTER_COMM_HANDLER               1 ///< Priority for Master Communication Handler.
#define TASTK_PRIO_MASTER_STATUS_CHECK_HANDLER       1 ///< Priority for Master Status Check Handler.
#define TASTK_PRIO_SLAVE_STATUS_OBSERVATION_HANDLING 1 ///< Priority for Slave Status Observation Handler.
#define TASTK_PRIO_SLAVE_RESTAT_STATUS               2 ///< Priority for Slave Restart Status Handler.
#define TASTK_PRIO_ECHO_SERVER_HANDLER               1 ///< Priority for Echo Server Handler.

/**
 * @brief Task execution time intervals (in milliseconds).
 *
 * These macros define the delay intervals for periodic task execution
 * across master and slave systems.
 */
#define TASTK_TIME_MASTER_COMM_HANDLER               10  ///< Time interval for Master Communication Handler.
#define TASTK_TIME_MASTER_STATUS_CHECK_HANDLER       500 ///< Time interval for Master Status Check Handler.
#define TASTK_TIME_SLAVE_STATUS_OBSERVATION_HANDLING 80  ///< Time interval for Slave Status Observation Handler.
#define TASTK_TIME_SLAVE_RESTAT_STATUS               10  ///< Time interval for Slave Restart Status Handler.
#define TASTK_TIME_ECHO_SERVER_HANDLER               80  ///< Time interval for Echo Server Handler.

#endif // THREAD_HANDLER_CFG_H
