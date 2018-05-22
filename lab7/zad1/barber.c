#include "shared.h"

int semaphoreId;
int sharedMemoryId;

void endBarber() {
    if (semctl(semaphoreId, 0, IPC_RMID) == -1) {
        printf("%sx \n", strerror(errno));
    }

    if (shmdt(block) == -1) {
        printf("%sy \n", strerror(errno));
    }

    if (shmctl(sharedMemoryId, IPC_RMID, NULL) == -1) {
        printf("%sz \n", strerror(errno));
    }

}

void handlerSIGTERM(pid_t pid) {
    exit(0);
}

int initBarber(int queueSize) {
    atexit(endBarber);
    signal(SIGTERM, handlerSIGTERM);

    key_t semaphoreKey = ftok("/home", projId);
    key_t sharedMemoryKey = ftok("/home/wojtek", projId);

    semaphoreId = semget(semaphoreKey, 1, IPC_CREAT|S_IRWXU);
    if (semaphoreId == -1) {
        printf("%sa \n", strerror(errno));
        return 1;
    }

    if (semctl(semaphoreId, 0, SETVAL, 1) == -1) {
        printf("%sb \n", strerror(errno));
        return 1;
    }


    sharedMemoryId = shmget(sharedMemoryKey, MAX_SHARED_MEMORY_SIZE, IPC_CREAT|S_IRWXU);
    if (sharedMemoryId == -1) {
        printf("%sk \n", strerror(errno));
        return 1;
    }

    block = shmat(sharedMemoryId, NULL, 0);
    if (block == (void *) -1) {
        printf("%sl \n", strerror(errno));
        return 1;
    }

    block->indexOfFirstCustomer = -1;
    block->indexOfLastCustomer = -1;
    block->realQueueSize = queueSize;
    block->isBarberSleeping = 1;

    return 0;
}

long getTime() {
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return tp.tv_nsec / 1000;
}

int getSemaphore() {
    struct sembuf buf[1];
    buf[0].sem_num = 0;
    buf[0].sem_op = -1;
    buf[0].sem_flg = 0;

    if (semop(semaphoreId, buf, 1) == -1) {
        printf("%s1 \n", strerror(errno));
        return 1;
    }
    return 0;
}

int returnSemaphore() {
    struct sembuf buf[1];
    buf[0].sem_num = 0;
    buf[0].sem_op = 1;
    buf[0].sem_flg = 0;

    if (semop(semaphoreId, buf, 1) == -1) {
        printf("%s2 \n", strerror(errno));
        return 1;
    }
    return 0;
}

int startBarber() {
    while(1) {
        if (getSemaphore() == 1) return 1;
        printf("Tutaj\n");
        if (returnSemaphore() == 1) return 1;
        return 0;
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