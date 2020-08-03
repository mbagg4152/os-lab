#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_CHARS 1024
#define MAX_LINES 400
#define TOTAL_MAX  MAX_CHARS * MAX_LINES

#define NO_PATH "#%?no#%?path#%?"
#define TAG  "my-uniq"

char *fileToArray(char *path);
char **splitFile(char *input);
void uniq(char *path);


int main(int argc, char **argv) {
    char *txtFileName;
    if (argc == 1) txtFileName = NO_PATH;
    else txtFileName = argv[1];
    uniq(txtFileName);
    return 0;
}


void uniq(char *path) {
    char splitData[MAX_LINES][MAX_CHARS];
    FILE *f;
    if (strcmp(path, NO_PATH) == 0) f = stdin;
    else f = fopen(path, "r");

    int count = 0;
    while (fgets(splitData[count], MAX_CHARS, f)) {
        splitData[count][strlen(splitData[count]) - 1] = '\n';
        count++;
    }

    int splitLen = count;
    for (int i = 0; i < splitLen; i++) {
        for (int j = i + 1; j < splitLen;) {
            if (strcmp(splitData[j], splitData[i]) == 0) {
                for (int k = j; k < splitLen; k++) {
                    strcpy(splitData[k], splitData[k + 1]);
                }
                splitLen--;
            } else {
                j++;
            }
        }
    }
    for (int i = 0; i < splitLen; i++) printf("%s", splitData[i]);
}