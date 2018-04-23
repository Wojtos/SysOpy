#include "shared.h"

pid_t pid;

int mainQueueID;
int clientQueueID;

int clientIDinMainQueue;

void deleteClientQueue() {
    msgctl(clientQueueID, IPC_RMID, NULL);

}

void handlerSIGINT() {
    exit(0);
}

int initClient() {
    pid = getpid();

    key_t clientQueueKey = ftok("/home", pid);
    key_t mainClientKey = ftok("/home", serverProjId);


    atexit(deleteClientQueue);
    signal(SIGINT, handlerSIGINT);

    clientQueueID = msgget(clientQueueKey, S_IRWXU|IPC_CREAT|IPC_EXCL);
    if (clientQueueID == -1) {
        printf("%s1 \n", strerror(errno));
        return 1;
    }

    mainQueueID = msgget(mainClientKey, S_IRWXU);
    if (mainQueueID == -1) {
        printf("%s2 \n", strerror(errno));
        return 1;
    }

    struct msgbuf toSend;

    toSend.pid = pid;
    toSend.queueKey = clientQueueKey;
    toSend.mtype = REGISTER;

    if (msgsnd(mainQueueID, &toSend, msgsz, 0) != 0) {
        printf("%s \n", strerror(errno));
        return 1;
    }

    printf("%s %d\n", "I am client, my queue id is: ", clientQueueID);

    struct msgbuf recived;


    if (msgrcv(clientQueueID, &recived, msgsz, REGISTER, 0) == 0) {
        printf("%s \n", strerror(errno));
        return 1;
    }

    clientIDinMainQueue = recived.idInMainQueue;
    printf("%s %d\n", "I am client, my number in mainQueue is: ", clientIDinMainQueue);
}

int main() {
    if (initClient() == 1) {
        return 1;
    }

    while (1);

    return 0;
}