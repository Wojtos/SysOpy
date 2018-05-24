#include "shared.h"

sem_t* semaphoreId;
int sharedMemoryId;

void endClients() {
    if (sem_close(semaphoreId) == -1) {
        printf("%sx \n", strerror(errno));
    }

    if (munmap(block, MAX_SHARED_MEMORY_SIZE) == -1) {
        printf("%sy \n", strerror(errno));
    }
}

void handlerSIGTERM(pid_t pid) {
    exit(0);
}

int initClients() {
    atexit(endClients);
    signal(SIGTERM, handlerSIGTERM);


    semaphoreId = sem_open(semaphoreName, O_RDWR);
    if (semaphoreId == SEM_FAILED) {
        printf("%sa \n", strerror(errno));
        return 1;
    }

    sharedMemoryId = shm_open(sharedMemoryName, O_RDWR, 0700);
    if (sharedMemoryId == -1) {
        printf("%sk \n", strerror(errno));
        return 1;
    }

    block = mmap(NULL, MAX_SHARED_MEMORY_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, sharedMemoryId, 0);
    if (block == (void *) -1) {
        printf("%sl \n", strerror(errno));
        return 1;
    }


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
    return block->indexOfLastCustomerToCut == -1 && block->indexOfLastCustomerToCut == block->indexOfLastCutCustomer;
}

pid_t shiftQueue() {
    block->indexOfLastCutCustomer++;
    return block->queue[block->indexOfLastCutCustomer];
}

int isQueueFull() {
    return block->indexOfLastCustomerToCut - block->indexOfLastCutCustomer >= block->realQueueSize;
}

void pushQueue() {
    block->indexOfLastCustomerToCut++;
    block->queue[block->indexOfLastCustomerToCut] = getpid();
}


int startClient() {
        if (getSemaphore() == 1) return 1;

        if(block->barberStatus == SLEEP) {
            block->chairStatus = SAT;
            block->barberStatus = AWAKEN;
            block->chair = getpid();

            printf("%ld: I have just waken barber up! - %d\n", getTime(), getpid());
            printf("%ld: I have just taken a sit! - %d\n", getTime(), getpid());
        } else {
            if (isQueueFull()) {
                printf("%ld: Waiting room was full! - %d\n", getTime(), getpid());
                if (returnSemaphore() == 1) return 1;
                return 0;
            } else {
                pushQueue();
                printf("%ld: I have just queued! - %d\n", getTime(), getpid());
            }
        }
        if (returnSemaphore() == 1) return 1;

        int takenSit = block->chair == getpid() ? 1 : 0;
        while(!takenSit) {
            if (getSemaphore() == 1) return 1;

            if (block->chair == getpid()) {
                takenSit = 1;
                block->chairStatus = SAT;
                block->indexOfLastCutCustomer++;
                printf("%ld: I have just taken a sit! - %d\n", getTime(), getpid());
            }

            if (returnSemaphore() == 1) return 1;
        }

        int finnishedCutting = 0;
        while(!finnishedCutting) {
            if (getSemaphore() == 1) return 1;

            if (block->barberStatus == FINISHEDCUTTING) {
                finnishedCutting = 1;
                block->chairStatus = EMPTY;
                printf("%ld: I have just had my hair cut! - %d\n", getTime(), getpid());
            }

            if (returnSemaphore() == 1) return 1;
        }
        return 0;

}

int startClients(int amountOfClients, int amountOfBarbers) {
    for (int i = 0; i < amountOfClients; ++i) {
        if (fork() == 0) {
            while(amountOfBarbers > 0) {
                if (startClient() == 1) exit(1);
                amountOfBarbers--;
                //printf("%d \n", amountOfBarbers);
            }
            exit(0);
        }
    }

    while(amountOfClients > 0) {
        int status;
        wait(&status);
        amountOfClients--;
        //printf("%s\n", strerror(WEXITSTATUS(status)));
    }
/*
    while(wait(NULL) != -1 && errno != ECHILD);
    printf("Tutaj jestem chlopaki! \n");
*/
    return 0;

}




int main(int argc, char *argv[]) {
    if (argc == 3) {
        int amountOfClients = atoi(argv[1]);
        int amountOfBarbers = atoi(argv[2]);
        if (initClients() == 1) {
            return 1;
        }
        if (startClients(amountOfClients, amountOfBarbers) == 1) {
            return 1;
        }
    }

    return 0;
}