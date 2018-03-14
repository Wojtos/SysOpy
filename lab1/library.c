#include "library.h"

#include <stdio.h>
#include <stdlib.h>

struct InfoAboutArray* allocateArray(size_t sizeOfBlock, size_t sizeOfArray, int dynamically) {
    if (dynamically) return allocateArrayDynamically(sizeOfBlock, sizeOfArray);
    else return allocateArrayStatically(sizeOfBlock, sizeOfArray);
}

void freeArray(struct InfoAboutArray* infoAboutArray){
    if (infoAboutArray->dynamicallyAlocated) freeArrayDynamically(infoAboutArray);
    else freeArrayStatically(infoAboutArray);
}

char* addBlock(struct InfoAboutArray* infoAboutArray, size_t index) {
    if (!infoAboutArray) {
        printf ("%s \n", "Firstly, you must allocate an array!");
        return NULL;
    }

    if (infoAboutArray->dynamicallyAlocated) return addBlockDynamically(infoAboutArray, index);
    else return addBlockStatically(infoAboutArray, index);
}

void deleteBlock(struct InfoAboutArray* infoAboutArray, int index) {
    if (!infoAboutArray) {
        printf ("%s \n", "Firstly, you must allocate an array!");
        return;
    }

    if (infoAboutArray->dynamicallyAlocated) deleteBlockDynamically(infoAboutArray, index);
    else deleteBlockStatically(infoAboutArray, index);
}

char* findTheClosestStringByASCII(struct InfoAboutArray* infoAboutArray, int index){
    if (!infoAboutArray) {
        printf ("%s \n", "Firstly, you must allocate an array!");
        return NULL;
    }

    if (infoAboutArray->dynamicallyAlocated) return findTheClosestStringByASCIIDynamically(infoAboutArray, index);
    else return findTheClosestStringByASCIIStatically(infoAboutArray, index);
}


struct InfoAboutArray* allocateArrayDynamically(size_t sizeOfBlock, size_t sizeOfArray) {
    struct InfoAboutArray* toReturn = (struct InfoAboutArray*) calloc(1, sizeof(*toReturn));

    toReturn->array = (char**) calloc(sizeOfArray, sizeof(char*));
    toReturn->ASCIIsum = (int*) calloc(sizeOfArray, sizeof(int));
    toReturn->sizeOfBlock = sizeOfBlock;
    toReturn->sizeOfArray = sizeOfArray;
    toReturn->dynamicallyAlocated = 1;

    if (!toReturn->array || !toReturn->ASCIIsum) {
        printf ("%s \n", "Memory overflow!");
        return NULL;
    }
    return toReturn;
}

void freeArrayDynamically(struct InfoAboutArray* infoAboutArray){
    free(infoAboutArray->array);
    free(infoAboutArray->ASCIIsum);
    free(infoAboutArray);
}

char* addBlockDynamically(struct InfoAboutArray* infoAboutArray, size_t index){
    if (!infoAboutArray || !infoAboutArray->array) {
        printf ("%s \n", "Firstly, you must allocate an array!");
        return NULL;
    }
    if (index >= infoAboutArray->sizeOfArray) {
        printf ("%s \n", "Index is too large!");
        return NULL;
    }
    if (infoAboutArray->array[index]) {
        printf ("%s \n", "This memory block has been allocated yet!");
        return NULL;
    }


    infoAboutArray->array[index] = (char*) calloc(infoAboutArray->sizeOfBlock + 1, sizeof(char));
    randstring(infoAboutArray->array[index], infoAboutArray->sizeOfBlock);
    infoAboutArray->array[index][infoAboutArray->sizeOfBlock] = '\0';

    infoAboutArray->ASCIIsum[index] = countASCIIsum(infoAboutArray->array[index], infoAboutArray->sizeOfBlock);

    return infoAboutArray->array[index];
}

void deleteBlockDynamically(struct InfoAboutArray* infoAboutArray, int index){
    if (!infoAboutArray || !infoAboutArray->array) {
        printf ("%s \n", "Firstly, you must allocate an array!");
        return;
    }

    free(infoAboutArray->array[index]);
    infoAboutArray->array[index] = NULL;
    infoAboutArray->ASCIIsum[index] = 0;
}

char* findTheClosestStringByASCIIDynamically(struct InfoAboutArray* infoAboutArray, int index){
    if (!infoAboutArray->array) {
        printf ("%s \n", "Firstly, you must allocate an array!");
        return NULL;
    }
    if (!infoAboutArray->array[index]) {
        printf ("%s \n", "This memory block has not been allocated yet!");
        return NULL;
    }

    int actualSum = infoAboutArray->ASCIIsum[index];
    size_t currentIndex = (index - 1)%infoAboutArray->sizeOfArray;

    for (int i = 0; i < infoAboutArray->sizeOfArray; i++) {
        if (infoAboutArray->ASCIIsum[i] && i != index) {
            if (abs(actualSum-infoAboutArray->ASCIIsum[i]) < abs(actualSum-infoAboutArray->ASCIIsum[currentIndex])) {
                currentIndex = i;
            }
        }
    }

    return infoAboutArray->array[currentIndex];
}

struct InfoAboutArray* allocateArrayStatically(size_t sizeOfBlock, size_t sizeOfArray) {
    if ((sizeOfArray * (sizeOfBlock + 1))* sizeof(char) + sizeOfArray * sizeof(int) > MEMORY_SIZE) {
        printf ("%s \n", "Memory overflow!");
        return NULL;
    }

    struct InfoAboutArray* toReturn = (struct InfoAboutArray*) calloc(1, sizeof(*toReturn));;

    toReturn->sizeOfBlock = sizeOfBlock;
    toReturn->sizeOfArray = sizeOfArray;
    toReturn->ASCIIsum = MEMORY;
    toReturn->array = (char**) (MEMORY + toReturn->sizeOfArray);
    toReturn->dynamicallyAlocated = 0;

    for (int i = 0; i < toReturn->sizeOfArray; i++) {
        toReturn->ASCIIsum[i] = 0;
    }

    return toReturn;
}

void freeArrayStatically(struct InfoAboutArray* infoAboutArray){
    free(infoAboutArray);
}


char* addBlockStatically(struct InfoAboutArray* infoAboutArray, size_t index){
    if (!infoAboutArray) {
        printf ("%s \n", "Firstly, you must allocate an array!");
        return NULL;
    }
    if (index >= infoAboutArray->sizeOfArray) {
        printf ("%s \n", "Index is too large!");
        return NULL;
    }
    if (infoAboutArray->ASCIIsum[index]) {
        printf ("%s \n", "This memory block has been allocated yet!");
        return NULL;
    }


    char* address = (char*)infoAboutArray->array + index * (infoAboutArray->sizeOfBlock + 1) * sizeof(char);

    randstring(address, infoAboutArray->sizeOfBlock);
    address[infoAboutArray->sizeOfBlock] = '\0';

    infoAboutArray->ASCIIsum[index] = countASCIIsum(address, infoAboutArray->sizeOfBlock);

    return address;
}

void deleteBlockStatically(struct InfoAboutArray* infoAboutArray, int index){
    if (!infoAboutArray) {
        printf ("%s \n", "Firstly, you must allocate an array!");
        return;
    }

    char* address = (char*)infoAboutArray->array + index * (infoAboutArray->sizeOfBlock + 1) * sizeof(char);

    *address = '\0';
    infoAboutArray->ASCIIsum[index] = 0;
}



char* findTheClosestStringByASCIIStatically(struct InfoAboutArray* infoAboutArray, int index){
    if (!infoAboutArray) {
        printf ("%s \n", "Firstly, you must allocate an array!");
        return NULL;
    }
    if (!infoAboutArray->ASCIIsum[index]) {
        printf ("%s \n", "This memory block has not been allocated yet!");
        return NULL;
    }

    int actualSum = infoAboutArray->ASCIIsum[index];
    size_t currentIndex = (index - 1)%infoAboutArray->sizeOfArray;

    for (size_t i = 0; i < infoAboutArray->sizeOfArray; i++) {
        if (infoAboutArray->ASCIIsum[i] && i != index) {
            if (abs(actualSum-infoAboutArray->ASCIIsum[i]) < abs(actualSum-infoAboutArray->ASCIIsum[currentIndex])) {
                currentIndex = i;
            }
        }
    }

    char* address = (char*)infoAboutArray->array + index * (infoAboutArray->sizeOfBlock + 1) * sizeof(char);

    return address;
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

int countASCIIsum(char* string, int sizeOfBlock){
    int sum = 0;
    if (string) {
        for (int i = 0; i < sizeOfBlock; i++) {
            sum += (int)string[i];
        }
    }

    return sum;
}
