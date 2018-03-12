#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include "library.c"

void initTime(clock_t* startTimeProccessor, struct tms* startTimeKernel) {
    *startTimeProccessor = clock();
    times(startTimeKernel);
}

void printTime(clock_t* startTimeProccessor, struct tms* startTimeKernel,
               clock_t* endTimeProccessor, struct tms* endTimeKernel,
               FILE* file) {
    *endTimeProccessor = clock();
    times(endTimeKernel);

    double realTime = (double) (*endTimeProccessor - *startTimeProccessor) / CLOCKS_PER_SEC;
    double userTime = (double) (endTimeKernel->tms_utime - startTimeKernel->tms_utime) / CLOCKS_PER_SEC;
    double kernelTime = (double) (endTimeKernel->tms_stime - startTimeKernel->tms_stime) / CLOCKS_PER_SEC;

    printf("Real time: %.6lf \n", realTime);
    printf("User time: %.6lf \n", userTime);
    printf("Kernel time: %.6lf \n", kernelTime);

    fprintf(file, "Real time: %.6lf \n", realTime);
    fprintf(file, "User time: %.6lf \n", userTime);
    fprintf(file, "Kernel time: %.6lf \n", kernelTime);
}

int main() {
    srand(time(NULL));

    clock_t* startTimeProccessor = malloc(sizeof(clock_t));
    clock_t* endTimeProccessor = malloc(sizeof(clock_t));;

    struct tms* startTimeKernel = malloc(sizeof(struct tms));
    struct tms* endTimeKernel = malloc(sizeof(struct tms));

    FILE *file = fopen("../raport2.txt", "a");
    if (!file) {
        printf ("%s \n", "Writing to file error!");
        return 1;
    }

    struct InfoAboutArray* infoAboutArray;
    struct InfoAboutArray* infoAboutArrays[10000];

    initTime(startTimeProccessor, startTimeKernel);
    for (int j = 0; j < 10000; ++j) {
        infoAboutArrays[j] =  allocateArray(1000, 100000, 1);
    }
    printf("%s \n", "Allocating dynamically 100000 arays with size of block equals 1000 and 100000 elements");
    fprintf(file, "%s \n", "Allocating dynamically 100000 arays with size of block equals 1000 and 100000 elements");
    printTime(startTimeProccessor, startTimeKernel, endTimeProccessor, endTimeKernel, file);

    initTime(startTimeProccessor, startTimeKernel);
    for (int j = 0; j < 10000; ++j) {
        freeArray(infoAboutArrays[j]);
    }
    printf("%s \n", "Freeing previous arrays");
    fprintf(file, "%s \n", "Freeing previous arrays");
    printTime(startTimeProccessor, startTimeKernel, endTimeProccessor, endTimeKernel, file);


    infoAboutArray =  allocateArray(100, 100000, 1);


    initTime(startTimeProccessor, startTimeKernel);
    for (size_t i = 0; i < 100000; ++i) {
        char* string = addBlock(infoAboutArray, i);
    }

    printf("%s \n", "Creating dynamically 100000 blocks");
    fprintf(file, "%s \n", "Creating dynamically 100000 blocks");
    printTime(startTimeProccessor, startTimeKernel, endTimeProccessor, endTimeKernel, file);


    initTime(startTimeProccessor, startTimeKernel);
    for (size_t i = 0; i < 100000; ++i) {
        deleteBlock(infoAboutArray, i);
        char* string = addBlock(infoAboutArray, i);
    }

    printf("%s \n", "Deleting and recreating dynamically 100000 blocks");
    fprintf(file, "%s \n", "Deleting and recreating dynamically 100000 blocks");
    printTime(startTimeProccessor, startTimeKernel, endTimeProccessor, endTimeKernel, file);

    initTime(startTimeProccessor, startTimeKernel);
    for (size_t i = 0; i < 1000; ++i) {
        findTheClosestStringByASCII(infoAboutArray, i);
    }

    printf("%s \n", "Finding the closest string by ASCII 1000 times in dynamical array");
    fprintf(file, "%s \n", "Finding the closest string by ASCII 1000 times in dynamical array");
    printTime(startTimeProccessor, startTimeKernel, endTimeProccessor, endTimeKernel, file);

    freeArray(infoAboutArray);

    initTime(startTimeProccessor, startTimeKernel);
    infoAboutArray = allocateArray(500, 10000, 0);

    printf("%s \n", "Allocating statically array with size of block equals 500 and 10000 elements");
    fprintf(file, "%s \n", "Allocating statically array with size of block equals 500 and 10000 elements");
    printTime(startTimeProccessor, startTimeKernel, endTimeProccessor, endTimeKernel, file);

    initTime(startTimeProccessor, startTimeKernel);
    for (size_t i = 0; i < 10000; ++i) {
        char* string = addBlock(infoAboutArray, i);
    }

    printf("%s \n", "Creating statically 10000 blocks");
    fprintf(file, "%s \n", "Creating statically 10000 blocks");
    printTime(startTimeProccessor, startTimeKernel, endTimeProccessor, endTimeKernel, file);

    initTime(startTimeProccessor, startTimeKernel);
    for (size_t i = 0; i < 10000; ++i) {
        deleteBlock(infoAboutArray, i);
        char* string = addBlock(infoAboutArray, i);
    }

    printf("%s \n", "Deleting and recreating statically 10000 blocks");
    fprintf(file, "%s \n", "Deleting and recreating statically 10000 blocks");
    printTime(startTimeProccessor, startTimeKernel, endTimeProccessor, endTimeKernel, file);


    initTime(startTimeProccessor, startTimeKernel);
    for (size_t i = 0; i < 1000; ++i) {
        findTheClosestStringByASCII(infoAboutArray, i);
    }

    printf("%s \n", "Finding the closest string by ASCII 1000 times in statical array");
    fprintf(file, "%s \n", "Finding the closest string by ASCII 1000 times in statical array");
    printTime(startTimeProccessor, startTimeKernel, endTimeProccessor, endTimeKernel, file);

    initTime(startTimeProccessor, startTimeKernel);
    freeArray(infoAboutArray);

    printf("%s \n", "Freeing previous array");
    fprintf(file, "%s \n", "Freeing previous array");
    printTime(startTimeProccessor, startTimeKernel, endTimeProccessor, endTimeKernel, file);

    free(startTimeProccessor);
    free(endTimeProccessor);
    free(startTimeKernel);
    free(endTimeKernel);

    fclose(file);

    return 0;
}

