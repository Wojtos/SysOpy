#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <memory.h>
#include <errno.h>

int main(int argc, char **argv) {
    if (argc > 1) {
        if (mkfifo(argv[1], 0777) == -1) {
            printf("%s \n", strerror(errno));
            return 1;
        }

        FILE *fifo;
        if ((fifo = fopen(argv[1], "r")) == NULL) {
            printf("%s \n", strerror(errno));
            return 1;
        }

        char buffer[4096];

        while (fgets(buffer, sizeof buffer, fifo)) {
            printf("%s", buffer);
        }
        fclose(fifo);
    }
    return 0;
}