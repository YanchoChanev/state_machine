#ifndef SLAVE_STATE_MACHINE_H
#define SLAVE_MAIN

typedef enum{
    IDEL,
    PROCESSING,
    ERROR,
    MAX_STATE_MASTER
} MasteStates;

typedef enum {
    SLEEP,
    ACTIVE,
    FAULT,
    MAX_STATE_SLAVE
} SlaveStates;

void statusChange(SlaveStates state);


#endif SLAVE_MAIN