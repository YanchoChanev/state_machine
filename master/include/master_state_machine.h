#ifndef MASTER_STATE_MACHINE_H
#define MASTER_STATE_MACHINE_H

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

void setNewState(MasteStates state);
MasteStates getCurrentState();

#endif MASTER_STATE_MACHINE_H