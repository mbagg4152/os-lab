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


char *fileToArray(char *path) {
    char *data;
    int counter = 0;
    int cap = 50;
    if (strcmp(path, NO_PATH) == 0) {
        if (stdin != NULL) {
            int used = 0;
            data = (char *) malloc(sizeof(char) * cap);
            int c = getchar();
            while (c != EOF) {
                if (used == cap) {
                    cap *= 2;
                    data = realloc(data, (sizeof(char) * cap));
                }
                data[used] = (char) c;
                used++;
                c = getchar();
            }
            return data;
        } else {
            printf("%s There was no file name supplied & stdin is empty. Nothing to do here", TAG);
            exit(1);
        }

    } else {
        FILE *txt = fopen(path, "r");
        if (txt == NULL) {  // required err message & action on file access failure
            printf("%s cannot open file '%s'\n", TAG, path);
            exit(1);
        }

        data = (char *) malloc(sizeof(char) * cap);
        while ((!feof(txt)) && (counter < TOTAL_MAX)) {
            char tmp;
            fscanf(txt, "%c", &tmp);
            if (counter == cap) {
                cap = cap * 2;
                data = (char *) realloc(data, (sizeof(char) * cap));
            }
            data[counter] = tmp;
            counter++;
        }
    }

    return data;
}


void uniq(char *path) {
    char *fileData = fileToArray(path);
    char **splitData = splitFile(fileData);
    int splitLen = 0;
    for (; splitLen < TOTAL_MAX; splitLen++) if (splitData[splitLen] == NULL) break;
    for (int i = 0; i < splitLen; i++) {
        char *chosen = splitData[i];
        if (chosen == NULL) break;
        for (int j = 0; j < splitLen; j++) {
            if (i != j) {
                char *comp = splitData[j];
                if (strcmp(comp, chosen) == 0) {
                    // there are excess null entries, if not excluded this messes with the array modifications
                    if (strcmp("\0", comp) != 0) {
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
    char **lines = malloc((MAX_LINES * (sizeof(char *))));
    for (int i = 0; i < MAX_LINES; i++) lines[i] = malloc(MAX_CHARS * (sizeof(char)));
    int lineCount = 0, charCount = 0, totalChars = 0;

    for (int i = 0; i < strlen(input) && input[i] != '\0'; i++) {
        char tmp = input[i];
        //printf("%c", tmp);
        if (lineCount == MAX_LINES) break;
        if (charCount < MAX_CHARS) {
            if (tmp == '\n') {
                lines[lineCount][charCount] = tmp;
                if (i < MAX_LINES + 1) lines[lineCount][charCount + 1] = '\0';
                lineCount++;
                totalChars++;
                charCount = 0;
            } else {
                if (i < MAX_LINES + 1) lines[lineCount][charCount + 1] = '\0';
                lines[lineCount][charCount] = tmp;
                totalChars++;
                charCount++;
            }
        } else lines[lineCount][charCount] = tmp;
    }

    lines = realloc(lines, (totalChars * lineCount) * sizeof(char *));
    return lines;
}