// thread_data.h
#ifndef THREAD_DATA_H
#define THREAD_DATA_H

#include <semaphore.h>

typedef struct {
    sem_t *x;  // Semaforo per sincronizzazione lettura/scrittura (es. readers-writers)
    sem_t *y;
} ThreadParams;

#endif