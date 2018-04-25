#include "shared.h"


int mainQueueID;

mqd_t numberOfClientQueues = 0;
mqd_t clientQueues[MAX_CLIENT_NUMBER];

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

void deleteQueues() {
    mq_close(mainQueueID);
    mq_unlink(SERVER_NAME);
    for (int i = 0; i < numberOfClientQueues; ++i) {
        mq_close(clientQueues[i]);
    }
}

void handlerSIGINT() {
    exit(0);
}

int initServer() {
    atexit(deleteQueues);
    signal(SIGINT, handlerSIGINT);

    struct mq_attr attr;
    attr.mq_msgsize = sizeof(struct msgbuf);
    attr.mq_maxmsg = 10;

    mainQueueID = mq_open(SERVER_NAME, O_RDONLY|O_CREAT|O_EXCL , S_IRWXU, &attr);
    if (mainQueueID == -1) {
        printf("%s \n", strerror(errno));
        return 1;
    }
}


int startServer() {
    struct msgbuf toSend;
    struct msgbuf recived;
    while (1) {
        if (mq_receive(mainQueueID, (char*)&recived, msgsz, NULL) == -1) {
            printf("%s \n", strerror(errno));
            return 1;
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

            char bufferName[100];
            sprintf(bufferName, "/clientQueue%d", recived.pid);

            struct mq_attr attr;
            attr.mq_msgsize = sizeof(struct msgbuf);
            attr.mq_maxmsg = 10;

            clientQueues[numberOfClientQueues] = mq_open(bufferName, O_WRONLY , S_IRWXU, &attr);
            numberOfClientQueues++;


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
            return 0;
        }

        if (mq_send(clientQueues[recived.idInMainQueue], (char*)&toSend, msgsz, 0) == -1) {
            printf("%s \n", strerror(errno));
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