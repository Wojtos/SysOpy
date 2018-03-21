#define __USE_XOPEN
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <memory.h>
#include <sys/stat.h>

void printMods(mode_t mode)
{
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
    printf("\n");
}

void printTime(time_t* time) {
    char buff[20];
    struct tm* timeinfo;

    timeinfo = localtime (time);
    strftime(buff, 20, "%Y-%m-%d %H:%M:%S", timeinfo);
    printf("%s \n",buff);
}

int compare(char* modifier, time_t time1, time_t time2) {
    if (modifier[0] == '<') {
        return time1 < time2 ? 1 : 0;
    } else if (modifier[0] == '>') {
        return time1 > time2 ? 1 : 0;
    }
    return time1 == time2 ? 1 : 0;
}

int searchFilesUsingBasicFunctions(char* path, char* modifier, time_t lastModificationTime) {
    char fullPath[4096];
    char currentPath[4096];
    struct stat infoAboutFile;

    DIR* mainDirectory = opendir(path);
    if (mainDirectory == NULL) {
        printf("Reading directory error! \n");
        return 1;
    }
    struct dirent* current;
    while (current = readdir(mainDirectory)) {
        if (current->d_name[0] != '.') {
            strcpy(currentPath,  path);
            if (currentPath[strlen(currentPath) - 1] != '/')
                strcat(currentPath, "/");
            strcat(currentPath, current->d_name);
            if (current->d_type == DT_REG) {
                realpath(currentPath, fullPath);
                if (stat(fullPath, &infoAboutFile) == -1) {
                    printf("Getting info about file error! \n");
                    return 1;
                }
                if (compare(modifier, lastModificationTime, infoAboutFile.st_mtime)) {
                    printf("%s \n", fullPath);
                    printf("Size in bites: %lu \n", infoAboutFile.st_size);
                    printMods(infoAboutFile.st_mode);
                    printTime(&(infoAboutFile.st_mtime));
                    printf("\n");
                }

            } else if (current->d_type == DT_DIR) {
                searchFilesUsingBasicFunctions(currentPath, modifier, lastModificationTime);
            }
        }
    }
    closedir(mainDirectory);
}


int main(int argc, char* argv[]) {
    if (argc == 5) {

        struct tm timeTmp;
        strptime(argv[3], "%Y-%m-%d_%H:%M:%S", &timeTmp);
        time_t lastModificationTime = mktime(&timeTmp);

        if ((strcmp(argv[4], "basic") == 0 || strcmp(argv[4], "basic ") == 0)) {
            searchFilesUsingBasicFunctions(argv[1], argv[2], lastModificationTime);
        } else if ((strcmp(argv[4], "nftw") == 0 || strcmp(argv[4], "nftw ") == 0)) {

        }
    }
    return 0;
}