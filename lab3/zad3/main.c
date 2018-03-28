#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <zconf.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdint.h>

void printTime(struct rusage* startRusage, struct rusage* endRusage) {
    struct rusage result;
    timersub(&(endRusage->ru_stime), &(startRusage->ru_stime), &(result.ru_stime));
    timersub(&(endRusage->ru_utime), &(startRusage->ru_utime), &(result.ru_utime));

    printf("User time: %ld.%06ld\n", result.ru_utime.tv_sec, result.ru_utime.tv_usec);
    printf("Kernel time: %ld.%06ld\n", result.ru_stime.tv_sec, result.ru_stime.tv_usec);


}

int main(int argc, char *argv[]){
    if (argc > 3) {
        int secondsCPULimit = atoi(argv[2]);
        int megabitesVirtualMemoryLimit = atoi(argv[3]);
        FILE *file = fopen(argv[1], "r");
        if (file == NULL) {
            printf("Reading file error! \n");
            printf("%s\n", strerror(errno));
            return 1;
        }

        char buffer[1024];
        char* args[6];
        struct rusage startRusage;
        struct rusage endRusage;

        while (fgets(buffer, sizeof buffer, file)) {
            args[0] = strtok(buffer, " \n");
            int i = 0;
            while (i < 5 && args[i]) {
                i++;
                args[i] = strtok(NULL, " \n");
            }

            printf("%s", "Executing:");
            for (int j = 0; j < i; ++j) {
                printf(" %s", args[j]);
            }
            printf("\n");


            if (getrusage(RUSAGE_CHILDREN, &startRusage) != 0) {
                printf("Reading time error! \n");
                printf("%s\n", strerror(errno));
                return 1;
            }

            pid_t childPid = fork();
            if (childPid == 0) {
                struct rlimit tmprlimit;
                if (getrlimit(RLIMIT_CPU, &tmprlimit) != 0) {
                    printf("Getting CPU limit error! \n");
                    printf("%s\n", strerror(errno));
                    return 1;
                }
                tmprlimit.rlim_max = (rlim_t)secondsCPULimit;
                if (tmprlimit.rlim_max < tmprlimit.rlim_cur) tmprlimit.rlim_cur = tmprlimit.rlim_max;
                if (setrlimit(RLIMIT_CPU, &tmprlimit) != 0) {
                    printf("Limiting CPU error! \n");
                    printf("%s\n", strerror(errno));
                    return 1;
                }

                if (getrlimit(RLIMIT_AS, &tmprlimit) != 0) {
                    printf("Getting virtual memory limit error! \n");
                    printf("%s\n", strerror(errno));
                    return 1;
                }
                tmprlimit.rlim_max = (rlim_t)(megabitesVirtualMemoryLimit * 1000000);
                if (tmprlimit.rlim_max < tmprlimit.rlim_cur) tmprlimit.rlim_cur = tmprlimit.rlim_max;
                if (setrlimit(RLIMIT_AS, &tmprlimit) != 0) {
                    printf("Limiting virtual memory error! \n");
                    printf("%s\n", strerror(errno));
                    return 1;
                }

                if (execvp(args[0], args) == -1) {
                    exit(errno);
                }
            } else {
                int statloc;
                wait(&statloc);
                if (getrusage(RUSAGE_CHILDREN, &endRusage) != 0) {
                    printf("Reading time error! \n");
                    printf("%s\n", strerror(errno));
                    return 1;
                }
                printTime(&startRusage, &endRusage);
                if (statloc != 0) {
                    printf("Executing function error! \n");
                    if (WIFEXITED(statloc)) printf("%s \n", strerror(WEXITSTATUS(statloc)));
                    if (WIFSIGNALED(statloc)) printf("%s \n", strsignal(WTERMSIG(statloc)));
                    return 1;
                }
                printf("\n");
            }


            for (int k = 0; k < i; ++k) {
                args[k] = NULL;
            }
        }

    }

    return 0;
}