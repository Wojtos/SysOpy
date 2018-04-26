#include "shared.h"

pid_t pid;

int mainQueueID;
int clientQueueID;

char bufferName[100];

int clientIDinMainQueue;

void deleteClientQueue() {
    mq_close(clientQueueID);
    mq_unlink(bufferName);

}

void handlerSIGINT() {
    exit(0);
}



int operationHandler(char* args[], int size) {
    struct msgbuf toSend;
    struct msgbuf recived;
    char buffer[1024];
    strcpy(buffer, "");

    for (int i = 0; i < size; ++i) {
        strcat(buffer, args[i]);
        if (i != size - 1) strcat(buffer, " ");
    }

    strcpy(toSend.buffer, buffer);
    toSend.idInMainQueue = clientIDinMainQueue;

    if (strcmp(args[0], "MIRROR") == 0 && size > 1) {
        toSend.mtype = MIRROR;

    } else if ((strcmp(args[0], "ADD") == 0 || strcmp(args[0], "MUL") == 0 || strcmp(args[0], "SUB") == 0
                || strcmp(args[0], "DIV") == 0) && size > 2) {

        toSend.mtype = CALC;

    } else if (strcmp(args[0], "TIME") == 0) {
        toSend.mtype = TIME;
    } else if (strcmp(args[0], "END") == 0) {
        toSend.mtype = END;
    } else {
        printf("%s \n", "Improper operation!");
    }

    if (mq_send(mainQueueID, (char*)&toSend, msgsz, 0) == -1) {
        printf("%s \n", strerror(errno));
        return 1;
    }

    if (toSend.mtype == END) return 2;


    if (mq_receive(clientQueueID, (char*)&recived, msgsz, NULL) == -1) {
        printf("%s \n", strerror(errno));
        return 1;
    }

    printf("%s: %s \n", args[0], recived.buffer);
    return 0;
}

int initClient() {
    pid = getpid();

    sprintf(bufferName, "/clientQueue%d", pid);

    atexit(deleteClientQueue);
    signal(SIGINT, handlerSIGINT);

    struct mq_attr attr;
    attr.mq_msgsize = sizeof(struct msgbuf);
    attr.mq_maxmsg = 10;
    
    clientQueueID = mq_open(bufferName, O_RDONLY|O_CREAT|O_EXCL , S_IRWXU, &attr);
    if (clientQueueID == -1) {
        printf("%s \n", strerror(errno));
        return 1;
    }

    mainQueueID = mq_open(SERVER_NAME, O_WRONLY , S_IRWXU, &attr);
    if (mainQueueID == -1) {
        printf("%s \n", strerror(errno));
        return 1;
    }

    struct msgbuf toSend;

    toSend.pid = pid;
    toSend.mtype = REGISTER;
    

    if (mq_send(mainQueueID, (char*)&toSend, msgsz, 0) == -1) {
        printf("%s \n", strerror(errno));
        return 1;
    }


    struct msgbuf recived;
    

    if (mq_receive(clientQueueID, (char*)&recived, msgsz, NULL) == -1) {
        printf("%s \n", strerror(errno));
        return 1;
    }


    clientIDinMainQueue = recived.idInMainQueue;
    return 0;
}

int startClient(char* filePath) {
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        printf("Reading file error! \n");
        printf("%s\n", strerror(errno));
        return 1;
    }

    char buffer[1024];
    char* args[6];


    while (fgets(buffer, sizeof buffer, file)) {
        args[0] = strtok(buffer, " \n");
        int i = 0;
        while (i < 5 && args[i]) {
            i++;
            args[i] = strtok(NULL, " \n");
        }

        int exitCode = operationHandler(args, i);
        if (exitCode == 1){
            return 1;
        } else if (exitCode == 2) {
            return 0;
        }


        for (int k = 0; k < i; ++k) {
            args[k] = NULL;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (initClient() == 1) {
            return 1;
        }

        printf("%s \n", "Ready to call!");

        return startClient(argv[1]);

    }
    return 0;
}