#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <zconf.h>
#include <stdlib.h>
#include <sys/wait.h>

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
        char* args[6];


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

            pid_t childPid = fork();
            if (childPid == 0) {
                if (execvp(args[0], args) == -1) {
                    exit(1);
                }
            } else {
                int statloc;
                wait(&statloc);
                if (statloc != 0) {
                    printf("Executing function error! \n");
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