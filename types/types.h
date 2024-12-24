#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

/**
 * @file types.h
 * @brief Common type definitions used across the system.
 *
 * This file defines commonly used data types, enumerations, and return values
 * to ensure consistency across different modules of the system.
 */

/**
 * @brief Enumeration of return values.
 *
 * Standard return values used across the system for function success and failure indications.
 */
typedef enum {
    RET_OK,    ///< Operation succeeded.
    RET_ERROR  ///< Operation failed.
} RetVal_t;

#endif // TYPES_H
