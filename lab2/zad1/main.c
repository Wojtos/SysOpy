#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <string.h>
#include <fcntl.h>
#include <zconf.h>

void initTime(clock_t* startTimeProccessor, struct tms* startTimeKernel) {
    *startTimeProccessor = clock();
    times(startTimeKernel);
}

void printTime(clock_t* startTimeProccessor, struct tms* startTimeKernel,
               clock_t* endTimeProccessor, struct tms* endTimeKernel,
               FILE* file, char* nameOfTest) {
    *endTimeProccessor = clock();
    times(endTimeKernel);

    double realTime = (double) (*endTimeProccessor - *startTimeProccessor) / CLOCKS_PER_SEC;
    double userTime = (double) (endTimeKernel->tms_utime - startTimeKernel->tms_utime) / CLOCKS_PER_SEC;
    double kernelTime = (double) (endTimeKernel->tms_stime - startTimeKernel->tms_stime) / CLOCKS_PER_SEC;

    printf("Name of test: %s\n", nameOfTest);
    printf("Real time: %.6lf \n", realTime);
    printf("User time: %.6lf \n", userTime);
    printf("Kernel time: %.6lf \n", kernelTime);

    fprintf(file, "Name of test: %s\n", nameOfTest);
    fprintf(file, "Real time: %.6lf \n", realTime);
    fprintf(file, "User time: %.6lf \n", userTime);
    fprintf(file, "Kernel time: %.6lf \n", kernelTime);
}

void randstring(char* string, size_t sizeOfBlock){
    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";

    if (string) {
        for (int i = 0; i < sizeOfBlock; i++) {
            int key = rand() % (int)(sizeof(charset) -1);
            string[i] = charset[key];
        }
    }
}

int generate(char* nameOfFile, int quantityOfRecords, size_t sizeOfRecord ) {
    FILE* file = fopen(nameOfFile, "w");
    if (file == NULL) {
        printf("Creating file error!\n");
        return 1;
    }
    char* buffer = malloc(sizeOfRecord);

    for (int i = 0; i < quantityOfRecords; ++i) {
        randstring(buffer, sizeOfRecord);
        if (!fwrite (buffer, sizeOfRecord, 1, file)) {
            printf("Writing to file error!\n");
            free(buffer);
            return 1;
        }
    }
    printf("Succes! Writed %d of records which size is %lu!\n", quantityOfRecords, sizeOfRecord);
    free(buffer);
    fclose(file);
    return 0;

}

int sortUsingCFunctions(char* nameOfFile, int quantityOfRecords, size_t sizeOfRecord ) {
    FILE* file = fopen(nameOfFile, "rb+");
    if (file == NULL) {
        printf("Creating file error!\n");
        return 1;
    }

    char* buffer1 = malloc(sizeOfRecord);
    char* buffer2 = malloc(sizeOfRecord);

    for (int i = 1; i < quantityOfRecords; ++i) {
        if (fseek(file, i * sizeOfRecord, 0) || !fread (buffer1, sizeOfRecord, 1, file)) {
            printf("Reading from file error!\n");
            free(buffer1);
            free(buffer2);
            return 1;
        }



        for (int j = i - 1; j >= 0; --j) {
            if (fseek(file, j * sizeOfRecord, 0) || !fread (buffer2, sizeOfRecord, 1, file)) {
                printf("Reading from file error!\n");
                free(buffer1);
                free(buffer2);
                return 1;
            }


            if ((int)buffer2[0] < (int)buffer1[0]) {
                break;
            } else {
                if (fseek(file, (j + 1) * sizeOfRecord, 0) || !fwrite (buffer2, sizeOfRecord, 1, file)) {
                    printf("Writing to file error!\n");
                    free(buffer1);
                    free(buffer2);
                    return 1;
                }
                if (fseek(file, j * sizeOfRecord, 0) || !fwrite (buffer1, sizeOfRecord, 1, file)) {
                    printf("Writing to file error!\n");
                    free(buffer1);
                    free(buffer2);
                    return 1;
                }
            }
        }
    }

    printf("Succes! Sorted %d of records which size is %lu using C functions!\n", quantityOfRecords, sizeOfRecord);
    free(buffer1);
    free(buffer2);
    fclose(file);
    return 0;


}

int copyUsingCFunctions(char* nameOfFile1, char* nameOfFile2, int quantityOfRecords, size_t sizeOfRecord ) {
    FILE* file1 = fopen(nameOfFile1, "rb+");
    FILE* file2 = fopen(nameOfFile2, "wb+");
    if (file1 == NULL || file2 == NULL) {
        printf("Creating or opening file error!\n");
        return 1;
    }

    char* buffer = malloc(sizeOfRecord);

    for (int i = 0; i < quantityOfRecords; ++i) {
        if (!fread (buffer, sizeOfRecord, 1, file1)) {
            printf("Reading from file error!\n");
            free(buffer);
            return 1;
        }

        if (!fwrite (buffer, sizeOfRecord, 1, file2)) {
            printf("Writing to file error!\n");
            free(buffer);
            return 1;
        }
    }




    printf("Succes! Copied %d of records which size is %lu using C functions!\n", quantityOfRecords, sizeOfRecord);
    free(buffer);
    fclose(file1);
    fclose(file2);
    return 0;


}

int copyUsingSysFunctions(char* nameOfFile1, char* nameOfFile2, int quantityOfRecords, size_t sizeOfRecord ) {
    int file1 = open(nameOfFile1, O_RDONLY);
    int file2 = open(nameOfFile1, O_WRONLY|O_CREAT);
    if (file1 == -1 || file2 == -1) {
        printf("Creating or opening file error!\n");
        return 1;
    }

    char* buffer = malloc(sizeOfRecord);

    for (int i = 0; i < quantityOfRecords; ++i) {
        if (!read(file1, buffer, sizeOfRecord)) {
            printf("Reading from file error!\n");
            free(buffer);
            return 1;
        }

        if (!write(file2, buffer, sizeOfRecord)) {
            printf("Writing to file error!\n");
            free(buffer);
            return 1;
        }
    }




    printf("Succes! Copied %d of records which size is %lu using Sys functions!\n", quantityOfRecords, sizeOfRecord);
    free(buffer);
    close(file1);
    close(file2);
    return 0;


}

int sortUsingSysFunctions(char* nameOfFile, int quantityOfRecords, size_t sizeOfRecord ) {
    int file = open(nameOfFile, O_RDWR);
    if (file == -1) {
        printf("Creating or opening file error!\n");
        return 1;
    }

    char* buffer1 = malloc(sizeOfRecord);
    char* buffer2 = malloc(sizeOfRecord);

    for (int i = 1; i < quantityOfRecords; ++i) {
        if (lseek(file, i * sizeOfRecord, SEEK_SET) < 0 || !read(file, buffer1, sizeOfRecord)) {
            printf("Reading from file error!\n");
            free(buffer1);
            free(buffer2);
            return 1;
        }



        for (int j = i - 1; j >= 0; --j) {
            if (lseek(file, j * sizeOfRecord, SEEK_SET) < 0 || !read(file, buffer2, sizeOfRecord)) {
                printf("Reading from file error!\n");
                free(buffer1);
                free(buffer2);
                return 1;
            }


            if ((int)buffer2[0] < (int)buffer1[0]) {
                break;
            } else {
                if (lseek(file, (j + 1) * sizeOfRecord, SEEK_SET) < 0 || !write(file, buffer2, sizeOfRecord)) {
                    printf("Writing to file error!\n");
                    free(buffer1);
                    free(buffer2);
                    return 1;
                }
                if (lseek(file, j * sizeOfRecord, SEEK_SET) < 0 || !write(file, buffer1, sizeOfRecord)) {
                    printf("Writing to file error!\n");
                    free(buffer1);
                    free(buffer2);
                    return 1;
                }
            }
        }
    }

    printf("Succes! Sorted %d of records which size is %lu using Sys functions!\n", quantityOfRecords, sizeOfRecord);
    free(buffer1);
    free(buffer2);
    close(file);
    return 0;


}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    if (argc == 5 && (strcmp(argv[1], "generate") == 0 || strcmp(argv[1], "generate ") == 0)) {
        int quantityOfRecords = atoi(argv[3]);
        size_t sizeOfRecord = (size_t)atoi(argv[4]);

        generate(argv[2], quantityOfRecords, sizeOfRecord);

    } else if (argc == 6 && (strcmp(argv[1], "sort") == 0 || strcmp(argv[1], "sort ") == 0)) {
        int quantityOfRecords = atoi(argv[3]);
        size_t sizeOfRecord = (size_t)atoi(argv[4]);

        if ((strcmp(argv[5], "c") == 0 || strcmp(argv[5], "c ") == 0)) {
            sortUsingCFunctions(argv[2], quantityOfRecords, sizeOfRecord);
        } else if ((strcmp(argv[5], "sys") == 0 || strcmp(argv[5], "sys ") == 0)) {
            sortUsingSysFunctions(argv[2], quantityOfRecords, sizeOfRecord);
        }

    } else if (argc == 7 &&(strcmp(argv[1], "copy") == 0 || strcmp(argv[1], "copy ") == 0)) {
        int quantityOfRecords = atoi(argv[4]);
        size_t sizeOfRecord = (size_t)atoi(argv[5]);

        if ((strcmp(argv[6], "c") == 0 || strcmp(argv[6], "c ") == 0)) {
            copyUsingCFunctions(argv[2], argv[3], quantityOfRecords, sizeOfRecord);
        } else if ((strcmp(argv[6], "sys") == 0 || strcmp(argv[6], "sys ") == 0)) {
            copyUsingSysFunctions(argv[2], argv[3], quantityOfRecords, sizeOfRecord);
        }
    } else {

        FILE* file = fopen("../wyniki.txt", "w");

        clock_t startTimeProccessor;
        clock_t endTimeProccessor;

        struct tms startTimeKernel;
        struct tms endTimeKernel;

        char* Byte4BlockSmall1 = "Byte4BlockSmall1";
        char* Byte4BlockSmall2 = "Byte4BlockSmall2";
        char* Byte4BlockBig1 = "Byte4BlockBig1";
        char* Byte4BlockBig2 = "Byte4BlockBig2";
        char* Byte512BlockSmall1 = "Byte512BlockSmall1";
        char* Byte512BlockSmall2 = "Byte512BlockSmall2";
        char* Byte512BlockBig1 = "Byte512BlockBig1";
        char* Byte512BlockBig2 = "Byte512BlockBig2";
        char* Byte4096BlockSmall1 = "Byte4096BlockSmall1";
        char* Byte4096BlockSmall2 = "Byte4096BlockSmall2";
        char* Byte4096BlockBig1 = "Byte4096BlockBig1";
        char* Byte4096BlockBig2 = "Byte4096BlockBig2";
        char* Byte8192BlockSmall1 = "Byte8192BlockSmall1";
        char* Byte8192BlockSmall2 = "Byte8192BlockSmall2";
        char* Byte8192BlockBig1 = "Byte8192BlockBig1";
        char* Byte8192BlockBig2 = "Byte8192BlockBig2";

        generate(Byte4BlockSmall1, 100, 4);
        generate(Byte4BlockBig1, 1000, 4);
        generate(Byte512BlockSmall1, 100, 512);
        generate(Byte512BlockBig1, 1000, 512);
        generate(Byte4096BlockSmall1, 100, 4096);
        generate(Byte4096BlockBig1, 1000, 4096);
        generate(Byte8192BlockSmall1, 100, 8192);
        generate(Byte8192BlockBig1, 1000, 8192);

        initTime(&startTimeProccessor, &startTimeKernel);
        copyUsingCFunctions(Byte4BlockSmall1, Byte4BlockSmall2, 100, 4);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "Copy Using C Functions 4 Bite Block Small");

        initTime(&startTimeProccessor, &startTimeKernel);
        copyUsingSysFunctions(Byte4BlockSmall1, Byte4BlockSmall2, 100, 4);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "Copy Using Sys Functions 4 Bite Block Small");

        initTime(&startTimeProccessor, &startTimeKernel);
        copyUsingCFunctions(Byte4BlockBig1, Byte4BlockBig2, 1000, 4);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "Copy Using C Functions 4 Bite Block Big");

        initTime(&startTimeProccessor, &startTimeKernel);
        copyUsingSysFunctions(Byte4BlockBig1, Byte4BlockBig2, 1000, 4);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "Copy Using Sys Functions 4 Bite Block Big");

        initTime(&startTimeProccessor, &startTimeKernel);
        copyUsingCFunctions(Byte512BlockSmall1, Byte512BlockSmall2, 100, 512);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "Copy Using C Functions 512 Bite Block Small");

        initTime(&startTimeProccessor, &startTimeKernel);
        copyUsingSysFunctions(Byte512BlockSmall1, Byte512BlockSmall2, 100, 512);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "Copy Using Sys Functions 512 Bite Block Small");

        initTime(&startTimeProccessor, &startTimeKernel);
        copyUsingCFunctions(Byte512BlockBig1, Byte512BlockBig2, 1000, 512);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "Copy Using C Functions 512 Bite Block Big");

        initTime(&startTimeProccessor, &startTimeKernel);
        copyUsingSysFunctions(Byte512BlockBig1, Byte512BlockBig2, 1000, 512);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "Copy Using Sys Functions 512 Bite Block Big");

        initTime(&startTimeProccessor, &startTimeKernel);
        copyUsingCFunctions(Byte4096BlockSmall1, Byte4096BlockSmall2, 100, 4096);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "Copy Using C Functions 4096 Bite Block Small");

        initTime(&startTimeProccessor, &startTimeKernel);
        copyUsingSysFunctions(Byte4096BlockSmall1, Byte4096BlockSmall2, 100, 4096);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "Copy Using Sys Functions 4096 Bite Block Small");

        initTime(&startTimeProccessor, &startTimeKernel);
        copyUsingCFunctions(Byte4096BlockBig1, Byte4096BlockBig2, 1000, 4096);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "Copy Using C Functions 4096 Bite Block Big");

        initTime(&startTimeProccessor, &startTimeKernel);
        copyUsingSysFunctions(Byte4096BlockBig1, Byte4096BlockBig2, 1000, 4096);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "Copy Using Sys Functions 4096 Bite Block Big");

        initTime(&startTimeProccessor, &startTimeKernel);
        copyUsingCFunctions(Byte8192BlockSmall1, Byte8192BlockSmall2, 100, 8192);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "Copy Using C Functions 8192 Bite Block Small");

        initTime(&startTimeProccessor, &startTimeKernel);
        copyUsingSysFunctions(Byte8192BlockSmall1, Byte8192BlockSmall2, 100, 8192);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "Copy Using Sys Functions 8192 Bite Block Small");

        initTime(&startTimeProccessor, &startTimeKernel);
        copyUsingCFunctions(Byte8192BlockBig1, Byte8192BlockBig2, 1000, 8192);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "Copy Using C Functions 8192 Bite Block Big");

        initTime(&startTimeProccessor, &startTimeKernel);
        copyUsingSysFunctions(Byte8192BlockBig1, Byte8192BlockBig2, 1000, 8192);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "Copy Using Sys Functions 8192 Bite Block Big");

        initTime(&startTimeProccessor, &startTimeKernel);
        sortUsingCFunctions(Byte4BlockSmall1, 100, 4);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "sort Using C Functions 4 Bite Block Small");

        initTime(&startTimeProccessor, &startTimeKernel);
        sortUsingSysFunctions(Byte4BlockSmall2, 100, 4);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "sort Using Sys Functions 4 Bite Block Small");

        initTime(&startTimeProccessor, &startTimeKernel);
        sortUsingCFunctions(Byte4BlockBig1, 1000, 4);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "sort Using C Functions 4 Bite Block Big");

        initTime(&startTimeProccessor, &startTimeKernel);
        sortUsingSysFunctions(Byte4BlockBig2, 1000, 4);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "sort Using Sys Functions 4 Bite Block Big");

        initTime(&startTimeProccessor, &startTimeKernel);
        sortUsingCFunctions(Byte512BlockSmall1, 100, 512);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "sort Using C Functions 512 Bite Block Small");

        initTime(&startTimeProccessor, &startTimeKernel);
        sortUsingSysFunctions(Byte512BlockSmall2, 100, 512);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "sort Using Sys Functions 512 Bite Block Small");

        initTime(&startTimeProccessor, &startTimeKernel);
        sortUsingCFunctions(Byte512BlockBig1, 1000, 512);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "sort Using C Functions 512 Bite Block Big");

        initTime(&startTimeProccessor, &startTimeKernel);
        sortUsingSysFunctions(Byte512BlockBig2, 1000, 512);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "sort Using Sys Functions 512 Bite Block Big");

        initTime(&startTimeProccessor, &startTimeKernel);
        sortUsingCFunctions(Byte4096BlockSmall1, 100, 4096);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "sort Using C Functions 4096 Bite Block Small");

        initTime(&startTimeProccessor, &startTimeKernel);
        sortUsingSysFunctions(Byte4096BlockSmall2, 100, 4096);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "sort Using Sys Functions 4096 Bite Block Small");

        initTime(&startTimeProccessor, &startTimeKernel);
        sortUsingCFunctions(Byte4096BlockBig1, 1000, 4096);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "sort Using C Functions 4096 Bite Block Big");

        initTime(&startTimeProccessor, &startTimeKernel);
        sortUsingSysFunctions(Byte4096BlockBig2, 1000, 4096);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "sort Using Sys Functions 4096 Bite Block Big");

        initTime(&startTimeProccessor, &startTimeKernel);
        sortUsingCFunctions(Byte8192BlockSmall1, 100, 8192);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "sort Using C Functions 8192 Bite Block Small");

        initTime(&startTimeProccessor, &startTimeKernel);
        sortUsingSysFunctions(Byte8192BlockSmall2, 100, 8192);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "sort Using Sys Functions 8192 Bite Block Small");

        initTime(&startTimeProccessor, &startTimeKernel);
        sortUsingCFunctions(Byte8192BlockBig1, 1000, 8192);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "sort Using C Functions 8192 Bite Block Big");

        initTime(&startTimeProccessor, &startTimeKernel);
        sortUsingSysFunctions(Byte8192BlockBig2, 1000, 8192);
        printTime(&startTimeProccessor, &startTimeKernel, &endTimeProccessor, &endTimeKernel, file,
                  "sort Using Sys Functions 8192 Bite Block Big");
        fclose(file);

    }

    return 0;
}