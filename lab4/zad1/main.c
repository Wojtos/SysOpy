#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <zconf.h>
#include <sys/wait.h>

pid_t globalChildPid = 0;

void run() {
    globalChildPid = fork();
    if (globalChildPid == 0) {
        char* tab[1];
        tab[0] = NULL;
        if (execvp("./../endlessTime.sh", tab) == -1) {
            exit(1);
        }
    }
}

void recivedSIGINT(int signo) {
    printf("Odebrano sygnał SIGINT! \n");
    kill(globalChildPid, SIGINT);
    exit(0);
}

void recivedSIGTSTP(int signo) {
    if (globalChildPid && (!waitpid(globalChildPid, NULL, WNOHANG))) {
        printf("Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu! \n");
        kill(globalChildPid, SIGKILL);
    } else {
        printf("Wznawiam program! \n");
        run();
    }
}



int main() {
    signal(SIGINT, recivedSIGINT);

    struct sigaction sigactionSIGTSTP;
    sigactionSIGTSTP.sa_handler = recivedSIGTSTP;
    sigemptyset(&sigactionSIGTSTP.sa_mask);
    sigactionSIGTSTP.sa_flags = 0;
    sigaction(SIGTSTP, &sigactionSIGTSTP, NULL);

    run();

    while (1);
}