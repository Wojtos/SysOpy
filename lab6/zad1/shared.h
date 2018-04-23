#ifndef ZAD1_SHARED_H
#define ZAD1_SHARED_H
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <memory.h>
#include <errno.h>
#include <stdlib.h>
#include <zconf.h>
#include <signal.h>

#define MAX_CLIENT_NUMBER 512
#define MAX_BUFFER_SIZE 2048

enum mtype {
    REGISTER = 1
};


struct msgbuf {
    long mtype;
    int idInMainQueue;
    pid_t pid;
    key_t queueKey;
    char buffer[MAX_BUFFER_SIZE];
};

const size_t msgsz = sizeof(struct msgbuf) - sizeof(long);


const char home[2] = "~";
const char serverProjId = '1';
#endif //ZAD1_SHARED_H
