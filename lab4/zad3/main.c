#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <signal.h>

int counterOfRecivedSignals = 0;

void recivedSIGUSR1(int sig, siginfo_t *siginfo, void *ucontext) {
    if (siginfo->si_pid == getppid()) {
        printf("I am child, recived SIGUSR1 number: %d and sending it back\n", counterOfRecivedSignals);
        counterOfRecivedSignals++;
        kill(getppid(), SIGUSR1);
    } else {
        printf("I am parent, recived SIGUSR1 number: %d \n", counterOfRecivedSignals);
        counterOfRecivedSignals++;
    }
}

void recivedSIGUSR2(int sig, siginfo_t *siginfo, void *ucontext) {
    if (siginfo->si_pid == getppid()) {
        printf("I am child, recived SIGUSR2, ending proccess\n");
        exit(0);
    }
}

void recivedSIGRTMIN(int sig, siginfo_t *siginfo, void *ucontext) {
    if (siginfo->si_pid == getppid()) {
        printf("I am child, recived SIGRTMIN number: %d and sending it back\n", counterOfRecivedSignals);
        counterOfRecivedSignals++;
        kill(siginfo->si_pid, SIGRTMIN);
    } else {
        printf("I am parent, recived SIGRTMIN number: %d \n", counterOfRecivedSignals);
        counterOfRecivedSignals++;
    }
}

void recivedSIGRTMAX(int sig, siginfo_t *siginfo, void *ucontext) {
    if (siginfo->si_pid == getppid()) {
        printf("I am child, recived SIGRTMAX, ending proccess\n");
        exit(0);
    }
}

int childProccess() {
    while (1);
}

int parentProccess1(int L, pid_t childPID) {
    for (int i = 0; i < L; ++i) {
        printf("I am parent, sending SIGUSR1 number: %d \n", i);
        kill(childPID, SIGUSR1);
    }
    printf("I am parent, sending SIGUSR2!\n");
    kill(childPID, SIGUSR2);
    sleep(10);
}

int parentProccess2(int L, pid_t childPID) {
    int counterOfSentSignals = 0;
    while (counterOfSentSignals < L) {
        if (counterOfSentSignals <= counterOfRecivedSignals) {
            printf("I am parent, sending SIGUSR1 number: %d \n", counterOfSentSignals);
            kill(childPID, SIGUSR1);
            counterOfSentSignals++;
        }
    }
    while (counterOfRecivedSignals != L);
    printf("I am parent, sending SIGUSR2!\n");
    kill(childPID, SIGUSR2);
    while (counterOfRecivedSignals < L);
}

int parentProccess3(int L, pid_t childPID) {
    for (int i = 0; i < L; ++i) {
        printf("I am parent, sending SIGRTMIN number: %d \n", i);
        kill(childPID, SIGRTMIN);
    }
    printf("I am parent, sending SIGRTMAX!\n");
    kill(childPID, SIGRTMAX);
    while (counterOfRecivedSignals < L);
}


int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Too few arguments! \n");
        return 1;
    }
    int L = atoi(argv[1]);
    int Type = atoi(argv[2]);

    struct sigaction sigactionSIGUSR1;
    sigactionSIGUSR1.sa_sigaction = recivedSIGUSR1;
    sigemptyset(&sigactionSIGUSR1.sa_mask);
    sigactionSIGUSR1.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sigactionSIGUSR1, NULL);

    struct sigaction sigactionSIGUSR2;
    sigactionSIGUSR2.sa_sigaction = recivedSIGUSR2;
    sigemptyset(&sigactionSIGUSR2.sa_mask);
    sigactionSIGUSR2.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR2, &sigactionSIGUSR2, NULL);

    struct sigaction sigactionSIGRTMIN;
    sigactionSIGRTMIN.sa_sigaction = recivedSIGRTMIN;
    sigemptyset(&sigactionSIGRTMIN.sa_mask);
    sigaddset(&sigactionSIGRTMIN.sa_mask, SIGRTMAX);
    sigactionSIGRTMIN.sa_flags = SA_SIGINFO;
    sigaction(SIGRTMIN, &sigactionSIGRTMIN, NULL);

    struct sigaction sigactionSIGRTMAX;
    sigactionSIGRTMAX.sa_sigaction = recivedSIGRTMAX;
    sigemptyset(&sigactionSIGRTMAX.sa_mask);
    sigactionSIGRTMAX.sa_flags = SA_SIGINFO;
    sigaction(SIGRTMAX, &sigactionSIGRTMAX, NULL);

    if (Type == 1) {
        pid_t childPID = fork();
        if (childPID == 0) {
            childProccess();
        } else {
            parentProccess1(L, childPID);
        }
    } else if (Type == 2) {
        pid_t childPID = fork();
        if (childPID == 0) {
            childProccess();
        } else {
            parentProccess2(L, childPID);
        }
    } else if (Type == 3) {
        pid_t childPID = fork();
        if (childPID == 0) {
            childProccess();
        } else {
            parentProccess3(L, childPID);
        }
    }
    return 0;
}