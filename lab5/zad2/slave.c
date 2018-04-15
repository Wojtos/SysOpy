#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <memory.h>
#include <errno.h>
#include <stdlib.h>
#include <zconf.h>

int file_exist (char *filename)
{
    struct stat buffer;
    return (stat (filename, &buffer) == 0);
}

int main(int argc, char **argv) {
    if (argc > 2) {
        pid_t PID = getpid();
        srand((unsigned int)PID);
        int N = atoi(argv[2]);
        FILE *fifo;
        FILE *date;

        char bufferForDate[40];
        char mainBuffer[50];


        printf("%ld \n", (long)PID);

        if (!file_exist(argv[1])) {
            printf("%s \n", "FIFO doesn't exist!");
            return 1;
        }

        if ((fifo = fopen(argv[1], "w")) == NULL) {
            printf("%s \n", strerror(errno));
            return 1;
        }

        for (int i = 0; i < N; ++i) {
            if ((date = popen("date", "r")) == NULL) {
                printf("%s \n", strerror(errno));
                return 1;
            }

            fgets(bufferForDate, 40, date);

            sprintf(mainBuffer, "%d %s", PID, bufferForDate);

            fputs(mainBuffer, fifo);
            fflush(fifo);
            fclose(date);

            sleep(rand() % 5);

        }


        fclose(fifo);

    }
    return 0;
}