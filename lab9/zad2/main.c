#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <zconf.h>
#include <bits/signum.h>
#include <signal.h>
#include <semaphore.h>

enum TypeOfComparing {
    LESS = -1,
    EQUAL,
    GREATER
};

int P, K, N, L, nk, greaterAmounfOfInfo, ended = 0;
enum TypeOfComparing typeOfComparing;
char** globalBuffer;
FILE* file;
pthread_t mainThread;

int vendorIndex = -1, consumerIndex = -1;
sem_t sem;

void recivedSIGINT(int signo) {
    if(file != NULL) fclose(file);
    if(globalBuffer != NULL) free(globalBuffer);
    exit(0);
}

int parseConfFile(char* confFilePath) {
    char buffer[8096];

    FILE* confFile = fopen(confFilePath, "r");
    if (confFile == NULL) {
        printf("%s\n", strerror(errno));
        return 1;
    }
    fgets(buffer, sizeof buffer, confFile); // 1 line P
    P = atoi(buffer);

    fgets(buffer, sizeof buffer, confFile); // 2 line K
    K = atoi(buffer);

    fgets(buffer, sizeof buffer, confFile); // 3 line N
    N = atoi(buffer);
    globalBuffer = calloc((size_t)N, sizeof(char*));

    fgets(buffer, sizeof buffer, confFile); // 4 line opening file
    file = fopen(strtok(buffer, "\n"), "r");
    if (file == NULL) {
        printf("%s\n", strerror(errno));
        return 1;
    }

    fgets(buffer, sizeof buffer, confFile); // 5 line L
    L = atoi(buffer);

    fgets(buffer, sizeof buffer, confFile); // 6 line comparing
    if (buffer[0] == '<') typeOfComparing = LESS;
    else if (buffer[0] == '=') typeOfComparing = EQUAL;
    else typeOfComparing = GREATER;

    fgets(buffer, sizeof buffer, confFile); // 7 line info level
    greaterAmounfOfInfo = atoi(buffer);

    fgets(buffer, sizeof buffer, confFile); // 8 line seconds
    nk = atoi(buffer);

    fclose(confFile);
    return 0;
}

int isGlobalBufferEmpty() {
    return consumerIndex == vendorIndex;
}
int isGlobalBufferFull() {
    return (consumerIndex == (vendorIndex + 1) % N) || (consumerIndex == -1 && vendorIndex + 1 == N);
}

int shouldBePrinted(char* string){
    return  ((typeOfComparing == LESS && strlen(string) < L) ||
            (typeOfComparing == EQUAL && strlen(string) == L) ||
            (typeOfComparing == GREATER && strlen(string) > L));
}

void* runVendor(void* tmp) {
    while(1) {
        sem_wait(&sem);
        if (isGlobalBufferFull()) {
            if (greaterAmounfOfInfo) printf("Vendor: Buffer is full!\n");
        } else {
            char buffer[8096];
            if (fgets(buffer, sizeof buffer, file) == NULL) {
                if (greaterAmounfOfInfo) printf("Vendor: My work is ended!\n");
                ended = 1;
                sem_post(&sem);
                return tmp;
            }

            if (greaterAmounfOfInfo) printf("Vendor: I read line from file and I am saving it to buffer!\n");

            char *line = calloc(strlen(buffer) + 1, sizeof(char));
            strcpy(line, buffer);

            vendorIndex++;
            vendorIndex %= N;

            //printf("%d\n", vendorIndex);

            globalBuffer[vendorIndex] = line;

        }
        sem_post(&sem);
    }
}

void* runConsumer(void* tmp) {
    while(1) {
        sem_wait(&sem);
        if (isGlobalBufferEmpty()) {
            if (ended == 1) {
                if (greaterAmounfOfInfo) printf("Consumer: My work is ended, so I am killing a proccess!\n");
                pthread_kill(mainThread, SIGTERM);
                return tmp;
            }
            if (greaterAmounfOfInfo) printf("Consumer: Buffer is empty!\n");
        } else {

            consumerIndex++;
            consumerIndex %= N;

            //printf("%d\n", consumerIndex);

            char *line = globalBuffer[consumerIndex];
            if (shouldBePrinted(line)) printf("Consumer: Correct line - %s", line);
            else if (greaterAmounfOfInfo) printf("Consumer: I read line from buffer, but it was incorrect!\n");
            free(line);
            globalBuffer[consumerIndex] = NULL;
        }
        sem_post(&sem);
    }
}

void runThreads() {
    sem_init(&sem, 0, 1);
    pthread_t* vendors = calloc((size_t)P, sizeof(pthread_t));
    pthread_t* consumers = calloc((size_t)K, sizeof(pthread_t));

    for (int i = 0; i < P; ++i) {
        pthread_create(&vendors[i], NULL, runVendor, NULL);
    }
    for (int i = 0; i < K; ++i) {
        pthread_create(&consumers[i], NULL, runConsumer, NULL);
    }

    if (nk == 0) {
        for (int i = 0; i < P; ++i) {
            pthread_join(vendors[i], NULL);
        }
        for (int i = 0; i < K; ++i) {
            pthread_join(consumers[i], NULL);
        }
    } else {
        sleep((unsigned int)nk);
        sem_wait(&sem);
        if (greaterAmounfOfInfo)
            printf("LORD: The path of the righteous man is beset on all sides by the iniquities of the selfish and the tyranny of evil men.\n Blessed is he, who in the name of charity and good will, shepherds the weak through the valley of darkness, for he is truly his brother’s keeper and the finder of lost children.\n And I will strike down upon thee with great vengeance and furious anger those who would attempt to poison and destroy my brothers.\n And you will know my name is the Lord when I lay my vengeance upon thee.\n");
        pthread_kill(mainThread, SIGTERM);
    }

    free(vendors);
    free(consumers);
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        signal(SIGINT, recivedSIGINT);
        signal(SIGTERM, recivedSIGINT);
        mainThread = pthread_self();

        if (parseConfFile(argv[1]) == 1) return 1;

        runThreads();

        fclose(file);
        free(globalBuffer);
    }
    return 0;
}