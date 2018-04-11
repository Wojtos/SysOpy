#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <signal.h>
#include <wait.h>
#include <memory.h>
#include <errno.h>

struct infoAboutChildrenProcesses {
    pid_t* itsPID;
    int* hasAsked;
    int size;
    int counterOfRequests;
    int limitOfRequests;
};

struct infoAboutChildrenProcesses* info;
__useconds_t microSecunds;

void recivedSIGUSR2(int sig, siginfo_t *siginfo, void *ucontext) {
    int realTimeSignal = SIGRTMIN + rand() % 32;
    kill(getppid(), realTimeSignal);
    exit(microSecunds / 1000000);
}

int childProccess() {
    srand(getpid());

    struct sigaction sigactionSIGUSR2;
    sigactionSIGUSR2.sa_sigaction = recivedSIGUSR2;
    sigemptyset(&sigactionSIGUSR2.sa_mask);
    sigactionSIGUSR2.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR2, &sigactionSIGUSR2, NULL);

    microSecunds = (__useconds_t)rand() % 10000000;
    usleep(microSecunds);
    kill(getppid(), SIGUSR1);

    pause();
}
void recivedSIGUSR1(int sig, siginfo_t *siginfo, void *ucontext) {
    int indexOfSignal = -1;
    for (int i = 0; i < info->size; ++i) {
        //printf("%d:%d\n", info->itsPID[i], siginfo->si_pid);
        if(info->itsPID[i] == siginfo->si_pid) {
            indexOfSignal = i;
            info->hasAsked[i] = 1;
            info->counterOfRequests++;
            printf("Recived signal from: %d\n", siginfo->si_pid);
            break;
        }
    }
    if (indexOfSignal == -1) {
        printf("Recived unkown signal! %d\n", siginfo->si_pid);
        return;
    }

    if (info->counterOfRequests == info->limitOfRequests) {
        for (int i = 0; i < info->size; ++i) {
            if(info->hasAsked[i]) {
                printf("Allowed signal %d to send real time signal!\n", info->itsPID[i]);
                kill(info->itsPID[i], SIGUSR2);
                break;
            }
        }
    } else if (info->counterOfRequests > info->limitOfRequests) {
        kill(siginfo->si_pid, SIGUSR2);
        printf("Allowed signal %d to send real time signal!\n", siginfo->si_pid);
        info->hasAsked[indexOfSignal] = 1;
    }
}

void recivedSIGRT(int sig, siginfo_t *siginfo, void *ucontext) {
    printf("Recived real time signal %d from %d! \n", sig, siginfo->si_pid);
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Too few arguments! \n");
        return 1;
    }
    int N = atoi(argv[1]);
    int M = atoi(argv[2]);

    struct sigaction sigactionSIGUSR1;
    sigactionSIGUSR1.sa_sigaction = recivedSIGUSR1;
    sigemptyset(&sigactionSIGUSR1.sa_mask);
    sigactionSIGUSR1.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sigactionSIGUSR1, NULL);

    struct sigaction sigactionSIGRT;
    sigactionSIGRT.sa_sigaction = recivedSIGRT;
    sigemptyset(&sigactionSIGRT.sa_mask);
    sigactionSIGRT.sa_flags = SA_SIGINFO;
    for (int j = 0; j < 32; ++j) {
        sigaction(SIGRTMIN + j, &sigactionSIGRT, NULL);
    }

    
    info = malloc(1);
    info->itsPID = malloc((size_t)(N * sizeof(pid_t)));
    info->hasAsked = malloc((size_t)(N * sizeof(int)));
    info->counterOfRequests = 0;
    info->limitOfRequests = M;
    info->size = N;

    for (int i = 0; i < N; ++i) {
        info->itsPID[i] = fork();
        if (info->itsPID[i] == 0) {
            childProccess();
            exit(0);
        } else {
            info->hasAsked[i] = 0;
            printf("Created a proccess which PID is %d! \n", info->itsPID[i]);
        }
    }

    for(int i = 0; i < info->size; i++) {
        while(1) {
            int status;
            pid_t childPID = waitpid(info->itsPID[i], &status, 0);
            if (childPID == -1) {
                if (errno == EINTR) continue;
                printf("%s \n", strerror(errno));
                return 1;
            }
            printf("Child proccess which PID was %d returned %d\n", childPID, status);
            break;
        }
    }

    return 0;
}