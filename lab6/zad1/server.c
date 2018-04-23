#include "shared.h"

int mainQueueID;

int numberOfClientQueues = 0;
int clientQueues[MAX_CLIENT_NUMBER];

void deleteMainQueue() {
    msgctl(mainQueueID, IPC_RMID, NULL);
}

void handlerSIGINT() {
    exit(0);
}

int initServer() {
    key_t mainQueueKey = ftok("/home", serverProjId);

    atexit(deleteMainQueue);
    signal(SIGINT, handlerSIGINT);

    mainQueueID = msgget(mainQueueKey, S_IRWXU|IPC_CREAT|IPC_EXCL);
    if (mainQueueID == -1) {
        printf("%s \n", strerror(errno));
        return 1;
    }
}


int startServer() {
    struct msgbuf toSend;
    struct msgbuf recived;
    while (1) {
        if (msgrcv(mainQueueID, &recived, msgsz, 0, 0) == 0) {
            printf("%s \n", strerror(errno));
            return 1;
        }
        if (recived.mtype == REGISTER) {
            toSend.mtype = REGISTER;
            toSend.idInMainQueue = numberOfClientQueues;

            clientQueues[numberOfClientQueues] = msgget(recived.queueKey, S_IRWXU);
            numberOfClientQueues++;

            printf("%s %d\n", "I am server, client queue id is: ", clientQueues[toSend.idInMainQueue]);

            if (msgsnd(clientQueues[toSend.idInMainQueue], &toSend, msgsz, 0) != 0) {
                printf("%s \n", strerror(errno));
                return 1;
            }


        }
    }
}

int main() {
    if (initServer() == 1) {
        return 1;
    }
    if (startServer() == 1) {
        return 1;
    }

    return 0;
}