#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef struct{
    uint8_t msg_id;
    uint8_t state;
} QueueMessage;

typedef enum{
    HANDEL_SLAVE_FAULT,
}MessageType;

#endif // TYPES_H