#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

#define maxChars 1024
#define maxLines 400

int absMax = maxChars * maxLines;

char *fileToArray(char *path);
char **splitFile(char *input);
void uniq(char *path);

int main(int argc, char **argv) {
    if (argc == 1) printf("no filename supplied as an argument.\n");
    else {
        uniq(argv[1]);
    }
    return 0;
}


char *fileToArray(char *path) {
    FILE *txt = fopen(path, "r");
    if (txt == NULL) {  // required err message & action on file access failure
        printf("my-uniq: cannot open file '%s'\n", path);
        exit(1);
    }

    int counter = 0;
    int cap = 50;
    char *data = (char *) malloc(sizeof(char) * cap);
    while ((!feof(txt)) && (counter < absMax)) {
        char tmp;
        fscanf(txt, "%c", &tmp);
        if (counter == cap) {
            cap = cap * 2;
            data = (char *) realloc(data, (sizeof(char) * cap));
        }
        data[counter] = tmp;
        counter++;
    }
    return data;
}


void uniq(char *path) {
    char *fileData = fileToArray(path);
    char **splitData = splitFile(fileData);
    int splitLen = 0;
    for (; splitLen < absMax; splitLen++) if (splitData[splitLen] == NULL) break;
    for (int i = 0; i < splitLen; i++) {
        char *chosen = splitData[i];
        if (chosen == NULL) break;
        for (int j = 0; j < splitLen; j++) {
            if (i != j) {
                char *comp = splitData[j];
                if (strcmp(comp, chosen) == 0) {
                    if (strcmp("\0", comp) != 0) { // there are excess null entries, if not excluded this messes with the array modifications
                        for (int k = j - 1; k < splitLen - 1; k++) splitData[k] = splitData[k + 1];
                    }
                }
            }

        }
    }

    for (int i = 0; i < splitLen; i++) printf("%s", splitData[i]);
    printf("\n");
}


char **splitFile(char *input) {
    char **lines = malloc((maxLines * (sizeof(char *))));
    for (int i = 0; i < maxLines; i++) lines[i] = malloc(maxChars * (sizeof(char)));
    int lineCount = 0, charCount = 0, totalChars = 0;

    for (int i = 0; i < strlen(input) && input[i] != '\0'; i++) {
        char tmp = input[i];
        //printf("%c", tmp);
        if (lineCount == maxLines) break;
        if (charCount < maxChars) {
            if (tmp == '\n') {
                lines[lineCount][charCount] = tmp;
                if (i < maxLines + 1) lines[lineCount][charCount + 1] = '\0';
                lineCount++;
                totalChars++;
                charCount = 0;
            } else {
                if (i < maxLines + 1) lines[lineCount][charCount + 1] = '\0';
                lines[lineCount][charCount] = tmp;
                totalChars++;
                charCount++;
            }
        } else lines[lineCount][charCount] = tmp;
    }

    lines = realloc(lines, (totalChars * lineCount) * sizeof(char *));
    return lines;
}