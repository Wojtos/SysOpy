#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "library.c"

int main() {
    srand(time(NULL));




    struct InfoAboutArray* infoAboutArray =  allocateArray(7, 100, 0);
    for (size_t i = 0; i <= 100; ++i) {
        char* string = addBlock(infoAboutArray, i);
        printf("%s \n", string);
        //deleteBlock(infoAboutArray, i);
        //printf("%s \n", string);
    }
    for (int i = 0; i < 100; ++i) {
        printf("%d \n", infoAboutArray->ASCIIsum[i]);
    }


    printf("%s \n", findTheClosestStringByASCII(infoAboutArray, 0));
    freeArray(infoAboutArray);
    infoAboutArray = NULL;
    printf("%s \n", findTheClosestStringByASCII(infoAboutArray, 0));


    return 0;
}