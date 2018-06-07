#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <memory.h>
#include <sys/time.h>
#include <pthread.h>
#include <tgmath.h>
#include <math.h>


#define maxImageSize 1000
#define maxFilterSize 100

struct threadInfo {
    int index;
    pthread_t thread;
};

int quantityOfThreads;

FILE* inputFile;
FILE* outputFile;
FILE* filterFile;

int width;
int height;
int filterSize;

u_int8_t inputPixels[maxImageSize][maxImageSize];
u_int8_t outputPixels[maxImageSize][maxImageSize];
double filterPixels[maxFilterSize][maxFilterSize];

void readInputFile() {
    char buffer[8096];

    fgets(buffer, sizeof buffer, inputFile); // 1 line P2
    fgets(buffer, sizeof buffer, inputFile); // 2 line comment
    fgets(buffer, sizeof buffer, inputFile); // 3 line width height

    width = atoi(strtok(buffer, " \n"));
    height = atoi(strtok(NULL, " \n"));

    fgets(buffer, sizeof buffer, inputFile); // 4 line max pixel size (is set to 255 in our program)


    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fscanf(inputFile, "%d", (int*)&inputPixels[i][j]);
        }
    }
}

void readFilterFile() {
    char buffer[8096];

    fgets(buffer, sizeof buffer, filterFile); // 1 line filterSize

    filterSize = atoi(strtok(buffer, " \n"));

    int indexHeight = 0;
    while (fgets(buffer, sizeof buffer, filterFile)) {
        filterPixels[indexHeight][0] = atof(strtok(buffer, " \n"));
        for (int i = 1; i < filterSize; ++i) {
            filterPixels[indexHeight][i] = atof(strtok(NULL, " \n"));
        }
        indexHeight++;
    }
}

void saveOutputFile(char* outputFileName) {
    fprintf(outputFile, "P2\n"); // 1 line P2
    fprintf(outputFile, "# %s\n", outputFileName); // 2 line comment
    fprintf(outputFile, "%d %d\n", width, height); // 3 line width height
    fprintf(outputFile, "255\n"); // 4 line max pixel size (is set to 255 in our program)


    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fprintf(outputFile, "%d ", outputPixels[i][j]);
        }
        fprintf(outputFile, "\n");
    }
}

int actionOnFiles(char* inputFileName, char* outputFileName, char* filterFileName) {
    inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL) {
        printf("%s\n", strerror(errno));
        return 1;
    }

    filterFile = fopen(filterFileName, "r");
    if (filterFile == NULL) {
        printf("%s\n", strerror(errno));
        return 1;
    }

    outputFile = fopen(outputFileName, "w");
    if (outputFile == NULL) {
        printf("%s\n", strerror(errno));
        return 1;
    }

    readInputFile();
    readFilterFile();

}

int saveStats(struct timeval* startTime) {
    FILE* statFile = fopen("./tests.txt", "a");
    if (statFile == NULL) {
        printf("%s\n", strerror(errno));
        return 1;
    }

    struct timeval endTime;
    gettimeofday(&endTime, NULL);
    timersub(&endTime, startTime, &endTime);


    fprintf(statFile, "Image width: %d, height: %d\n", height, width);
    fprintf(statFile, "Filter size: %d \n", filterSize);
    fprintf(statFile, "Quantity of threads: %d \n", quantityOfThreads);
    fprintf(statFile, "Duration in secunds: %ld.%06ld \n\n", endTime.tv_sec, endTime.tv_usec);
    fclose(statFile);

    return 0;
}

int checkBorders(int min, int max, int value) {
    if (value <= min) return min;
    if (value >= max) return max;
    return value;
}

double countPixel(int x, int y) {
    double counter = 0;
    for (int i = 1; i <= filterSize; ++i) {
        for (int j = 1; j <= filterSize; ++j) {
            int indexi = checkBorders(1, height, x - ((filterSize+1)/2) + i) - 1;
            int indexj = checkBorders(1, width, y - ((filterSize+1)/2) + j) - 1;
            counter += inputPixels[indexi][indexj] * filterPixels[i][j];
        }
    }
    return round(counter);
}

void* threadWork(void* tmp) {
    struct threadInfo* info;
    info = (struct threadInfo*)tmp;

    int startingHeight = (height * info->index)/quantityOfThreads;
    int endingHeight = (height * (info->index + 1))/quantityOfThreads;

    printf("%d: %d -> %d\n", info->index, startingHeight, endingHeight);

    for (int i = startingHeight; i < endingHeight; ++i) {
        for (int j = 0; j < width; ++j) {
            outputPixels[i][j] = (__uint8_t)countPixel(i, j);
        }
    }

    return 0;

}

int main(int argc, char *argv[]) {
    if (argc == 5) {
        quantityOfThreads = atoi(argv[1]);
        char* inputFileName = argv[2];
        char* outputFileName = argv[3];
        char* filterFileName = argv[4];

        if (actionOnFiles(inputFileName, outputFileName, filterFileName) == 1) {
            return 1;
        }

        struct threadInfo* threadsInfo = malloc(quantityOfThreads * sizeof(struct threadInfo));

        struct timeval startTime;
        gettimeofday(&startTime, NULL);

        for (int i = 0; i < quantityOfThreads; ++i) {
            threadsInfo[i].index = i;
            pthread_create(&threadsInfo[i].thread, NULL, threadWork, &threadsInfo[i]);
        }

        for (int i = 0; i < quantityOfThreads; ++i) {
            pthread_join(threadsInfo[i].thread, NULL);
        }

        if (saveStats(&startTime) == 1) {
            return 1;
        }

        saveOutputFile(outputFileName);


        fclose(inputFile);
        fclose(outputFile);
        fclose(filterFile);

        free(threadsInfo);

    }

    return 0;
}