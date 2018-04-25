#ifndef ZAD1_SHARED_H
#define ZAD1_SHARED_H

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <sys/types.h>
#include <memory.h>
#include <errno.h>
#include <stdlib.h>
#include <zconf.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>


#define MAX_CLIENT_NUMBER 512
#define MAX_BUFFER_SIZE 2048
#define SERVER_NAME "/serverQueue"

enum mtype {
    REGISTER = 1,
    MIRROR,
    CALC,
    TIME,
    END
};


struct msgbuf {
    long mtype;
    int idInMainQueue;
    pid_t pid;
    char buffer[MAX_BUFFER_SIZE];
};

const size_t msgsz = sizeof(struct msgbuf);


const char home[2] = "~";
const char serverProjId = '1';
#endif //ZAD1_SHARED_H
