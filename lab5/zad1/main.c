#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <zconf.h>
#include <stdlib.h>
#include <sys/wait.h>

int execFunction(char* args[6][6], int numberOfPipes) {
    printf("\n%s", "Executing:");
    for (int j = 0; j < numberOfPipes; ++j) {
        if (j != 0) printf(" |");
        for (int i = 0; i < 6 && args[j][i]; ++i) {
            printf(" %s", args[j][i]);
        }
    }
    printf("\n");
/*
    for (int j = 0; j < numberOfPipes; ++j) {
        for (int i = 0; i < 6 && args[j][i]; ++i) {
            printf("%s ", args[j][i]);
        }
        printf("\n");
    }
*/
    int fd[6][2];
    for (int k = 0; k < numberOfPipes; ++k) {
        if (k < numberOfPipes - 1) pipe(fd[k]);
        pid_t childPid = fork();
        if (childPid == 0) {
            if (k != 0) {
                if (dup2(fd[k-1][0], STDIN_FILENO) < 0)
                    printf("dup2 STDIN_FILENO error! \n");
            }
            if (k < numberOfPipes - 1) {
                close(fd[k][0]);
                if(dup2(fd[k][1], STDOUT_FILENO) < 0)
                    printf("dup2 STDOUT_FILENO error! \n");
            }
            if (execvp(args[k][0], args[k]) == -1) {
                printf("%s \n", strerror(errno));
                exit(1);
            }
        } else {
            if (k < numberOfPipes - 1) {
                close(fd[k][1]);
                wait(NULL);
            }
            if (k != 0) close(fd[k-1][0]);
        }
    }

    int statloc;
    wait(&statloc);
    if (statloc != 0) {
        printf("Executing function error! \n");
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[]){
    if (argc > 1) {
        printf("%s\n", argv[1]);
        FILE *file = fopen(argv[1], "r");
        if (file == NULL) {
            printf("Reading file error! \n");
            printf("%s\n", strerror(errno));
            return 1;
        }

        char buffer[1024];
        char buffer2[6][1024];
        char* args[6][6];
        int numberOfPipes;

        while (fgets(buffer, sizeof buffer, file)) {
            for (int k = 0; k < 6; ++k) {
                for (int i = 0; i < 6; ++i) {
                    args[k][i] = NULL;
                }
            }

            args[0][0] = strtok(buffer, "|\n");
            int i = 0;
            while (i < 5 && args[i][0]) {
                i++;
                args[i][0]= strtok(NULL, "|\n");
            }
            numberOfPipes = i;
/*
            for (int j = 0; j < numberOfPipes; ++j) {
                printf("%s ", args[j][0]);
                printf("\n");

            }
*/
            for (int j = 0; j < i; ++j) {
                strcpy(buffer2[j], args[j][0]);
                args[j][0] = strtok(buffer2[j], " \n");
                int k = 0;
                while (k < 5 && args[j][k]) {
                    k++;
                    args[j][k] = strtok(NULL, " \n");
                }
            }

            if (execFunction(args, numberOfPipes) == 1) {
                return 1;
            }
        }
    }
    return 0;
}