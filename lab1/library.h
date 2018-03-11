#ifndef LAB1_LIBRARY_H
#define LAB1_LIBRARY_H
#define MEMORY_SIZE 5000

#include <glob.h>

struct InfoAboutArray {
    char** array;
    int* ASCIIsum;
    size_t sizeOfBlock;
    size_t sizeOfArray;
    int dynamicallyAlocated;
};

int MEMORY[MEMORY_SIZE];

struct InfoAboutArray* allocateArray(size_t sizeOfBlock, size_t sizeOfArray, int dynamically);
void freeArray(struct InfoAboutArray* infoAboutArray);
char* addBlock(struct InfoAboutArray* infoAboutArray, size_t index);
void deleteBlock(struct InfoAboutArray* infoAboutArray, int index);
char* findTheClosestStringByASCII(struct InfoAboutArray* infoAboutArray, int index);

struct InfoAboutArray* allocateArrayDynamically(size_t sizeOfBlock, size_t sizeOfArray);
void freeArrayDynamically(struct InfoAboutArray* infoAboutArray);
char* addBlockDynamically(struct InfoAboutArray* infoAboutArray, size_t index);
void deleteBlockDynamically(struct InfoAboutArray* infoAboutArray, int index);
char* findTheClosestStringByASCIIDynamically(struct InfoAboutArray* infoAboutArray, int index);


struct InfoAboutArray* allocateArrayStatically(size_t sizeOfBlock, size_t sizeOfArray);
void freeArrayStatically(struct InfoAboutArray* infoAboutArray);
char* addBlockStatically(struct InfoAboutArray* infoAboutArray, size_t index);
void deleteBlockStatically(struct InfoAboutArray* infoAboutArray, int index);
char* findTheClosestStringByASCIIStatically(struct InfoAboutArray* infoAboutArray, int index);


void randstring(char* string, size_t sizeOfBlock);
int countASCIIsum(char* string, int sizeOfBlock);
#endif