#include <time.h>
#include "shared.h"

int mainQueueID;

int numberOfClientQueues = 0;
int clientQueues[MAX_CLIENT_NUMBER];

char* strrev(char *str)
{
    char *p1, *p2;

    if (! str || ! *str)
        return str;
    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
    {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return str;
}

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
    int end = 0;
    while (1) {
        if (!end) {
            if (msgrcv(mainQueueID, &recived, msgsz, 0, 0) == -1) {
                printf("%s1 \n", strerror(errno));
                return 1;
            }
        } else {
            if (msgrcv(mainQueueID, &recived, msgsz, 0, IPC_NOWAIT) == -1) {
                if (errno == ENOMSG)
                    return 0;
                printf("%s1 \n", strerror(errno));
                return 1;
            }
        }

        char* args[6];
        strcpy(toSend.buffer, "");

        args[0] = strtok(recived.buffer, " \n");
        int i = 0;
        while (i < 5 && args[i]) {
            i++;
            args[i] = strtok(NULL, " \n");
        }

        if (recived.mtype == REGISTER) {
            toSend.mtype = REGISTER;
            toSend.idInMainQueue = numberOfClientQueues;
            recived.idInMainQueue = numberOfClientQueues;


            clientQueues[numberOfClientQueues] = msgget(recived.queueKey, S_IRWXU);
            numberOfClientQueues++;

            printf("%s %d\n", "I am server, client queue id is: ", clientQueues[toSend.idInMainQueue]);

        } else if (recived.mtype == MIRROR) {
            toSend.mtype = MIRROR;
            strcpy(toSend.buffer, strrev(args[1]));

        } else if (recived.mtype == CALC) {
            toSend.mtype = CALC;

            int a = atoi(args[1]);
            int b = atoi(args[2]);

            if (strcmp(args[0], "ADD") == 0) {
                sprintf(toSend.buffer, "%d", a + b);
            } else if (strcmp(args[0], "MUL") == 0) {
                sprintf(toSend.buffer, "%d", a * b);
            } else if (strcmp(args[0], "SUB") == 0) {
                sprintf(toSend.buffer, "%d", a - b);
            } else if (strcmp(args[0], "DIV") == 0) {
                sprintf(toSend.buffer, "%d", a / b);
            }

        } else if (recived.mtype == TIME) {
            toSend.mtype = TIME;

            time_t now = time(NULL);
            struct tm *t = localtime(&now);


            strftime(toSend.buffer, 100, "%d %m %Y %H:%M", t);
        } else if (recived.mtype == END) {
            end = 1;
        }

        if (recived.mtype != END && msgsnd(clientQueues[recived.idInMainQueue], &toSend, msgsz, 0) == -1) {
            printf("%s2 \n", strerror(errno));
            return 1;
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