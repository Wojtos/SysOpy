#ifndef ZAD1_SHARED_H
#define ZAD1_SHARED_H
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <memory.h>
#include <errno.h>
#include <stdlib.h>
#include <zconf.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>


#define MAX_CLIENT_NUMBER 512
#define MAX_SHARED_MEMORY_SIZE 8192
#define MAX_QUEUE_SIZE 1024

const int projId = 60;

enum BarberStatus {
    SLEEP,
    AWAKEN,
    HASINVITED,
    STARTEDTOCUT,
    FINISHEDCUTTING
};

enum ChairStatus {
    EMPTY,
    SAT
};

struct sharedMemoryBlock {
    pid_t queue[MAX_QUEUE_SIZE];
    pid_t chair;
    int indexOfLastCutCustomer;
    int indexOfLastCustomerToCut;
    int realQueueSize;
    enum BarberStatus barberStatus;
    enum ChairStatus chairStatus;

} *block;



#endif //ZAD1_SHARED_H