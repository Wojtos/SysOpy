#include "shared.h"

sem_t* semaphoreId;
int sharedMemoryId;

void endBarber() {
    if (sem_close(semaphoreId) == -1) {
        printf("%sx \n", strerror(errno));
    }

    if (munmap(block, MAX_SHARED_MEMORY_SIZE) == -1) {
        printf("%sy \n", strerror(errno));
    }

    if (shm_unlink(sharedMemoryName) == -1) {
        printf("%sz \n", strerror(errno));
    }

}

void handlerSIGTERM(pid_t pid) {
    exit(0);
}

int initBarber(int queueSize) {
    atexit(endBarber);
    signal(SIGTERM, handlerSIGTERM);
    signal(SIGINT, handlerSIGTERM);

    semaphoreId = sem_open(semaphoreName, O_RDWR|O_CREAT, 0700, 1);
    if (semaphoreId == SEM_FAILED) {
        printf("%sa \n", strerror(errno));
        return 1;
    }

    sharedMemoryId = shm_open(sharedMemoryName, O_RDWR|O_CREAT, 0700);
    if (sharedMemoryId == -1) {
        printf("%sk1 \n", strerror(errno));
        return 1;
    }

    if (ftruncate(sharedMemoryId, MAX_SHARED_MEMORY_SIZE) == -1) {
        printf("%sk2 \n", strerror(errno));
        return 1;
    }

    block = mmap(NULL, MAX_SHARED_MEMORY_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, sharedMemoryId, 0);
    if (block == (void *) -1) {
        printf("%sl \n", strerror(errno));
        return 1;
    }

    block->indexOfLastCustomerToCut = -1;
    block->indexOfLastCutCustomer = -1;
    block->realQueueSize = queueSize;
    block->barberStatus = SLEEP;
    block->chairStatus = EMPTY;

    return 0;
}

long getTime() {
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return tp.tv_nsec / 1000;
}

int getSemaphore() {
    if (sem_wait(semaphoreId) == -1) {
        printf("%s1 \n", strerror(errno));
        return 1;
    }
    return 0;
}

int returnSemaphore() {
    if (sem_post(semaphoreId) == -1) {
        printf("%s2 \n", strerror(errno));
        return 1;
    }
    return 0;
}

int isQueueEmpty() {
    return block->indexOfLastCustomerToCut == -1 || block->indexOfLastCustomerToCut == block->indexOfLastCutCustomer;
}

pid_t shiftQueue() {
    return block->queue[block->indexOfLastCutCustomer + 1];
}

int startBarber() {
    while(1) {
        if (getSemaphore() == 1) return 1;

        if(block->barberStatus == SLEEP) {
            //printf("%ld: I have just awaken!\n", getTime());

            if(isQueueEmpty()) {
                //printf("%ld: I have just fallen asleep!\n", getTime());
            } else {
                block->barberStatus = HASINVITED;
                block->chair = shiftQueue();
                printf("%ld: I have just invited %d to cut!\n", getTime(), block->chair);
            }
        } else if(block->barberStatus == HASINVITED) {
            if (block->chairStatus == SAT) {
                block->barberStatus = STARTEDTOCUT;
                printf("%ld: I have just started cutting %d\n", getTime(), block->chair);
            }
        } else if(block->barberStatus == STARTEDTOCUT) {
            block->barberStatus = FINISHEDCUTTING;
            printf("%ld: I have just finished cutting %d\n", getTime(), block->chair);
        } else if(block->barberStatus == FINISHEDCUTTING) {
            if (block->chairStatus == EMPTY) {
                if(isQueueEmpty()) {
                    block->barberStatus = SLEEP;
                    printf("%ld: I have just fallen asleep!\n", getTime());
                } else {
                    block->barberStatus = HASINVITED;
                    block->chair = shiftQueue();
                    printf("%ld: I have just invited %d to cut!\n", getTime(), block->chair);
                }
            }
        } else if(block->barberStatus == AWAKEN) {
            printf("%ld: I have just awaken!\n", getTime());
            if (block->chairStatus == SAT) {
                block->barberStatus = STARTEDTOCUT;
                printf("%ld: I have just started cutting %d\n", getTime(), block->chair);
            }
        }

        if (returnSemaphore() == 1) return 1;
    }
}




int main(int argc, char *argv[]) {
    if (argc == 2) {
        int queueSize = atoi(argv[1]);
        if (initBarber(queueSize) == 1) {
            return 1;
        }
        if (startBarber() == 1) {
            return 1;
        }
    }

    return 0;
}