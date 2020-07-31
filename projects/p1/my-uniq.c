#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define maxCharsPerLine 750
#define maxLines 400

int absMax = maxCharsPerLine * maxLines;

char **splitFile(char *input);
void uniq(char *path);
char *fileToArray(char *path);
void db(char *msg);
void dbi(int msg);


int main(int argc, char **argv) {
    if (argc == 1) printf("no filename supplied as an argument.\n");
    else {
        uniq(argv[1]);
    }
    return 0;
}


char *fileToArray(char *path) {
    FILE *txt = fopen(path, "r");
    if (txt == NULL) {
        printf("file %s could not be found or is not accessible\n", path);
        return "";
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


void db(char *msg) { printf("%s\n", msg); }


void dbi(int msg) { printf("%d\n", msg); }


void uniq(char *path) {
    char *fileData = fileToArray(path);
    //printf("%s\n", fileData);
    char **splitData = splitFile(fileData);
    char **splitCopy = splitFile(fileData);
    int splitLen = 0;
    for (; splitLen < absMax; splitLen++) if (splitData[splitLen] == NULL) break;
    int copyLen = splitLen - 1;
    for (int i = 0; i < maxLines; i++) {
        char *chosen = splitData[i];

//        db("1");
//        for (int j = 0; j < maxLines; j++) {
//            if (i == 0) j = 0;
//            db("2");
//            if (j != i) { // don't want to compare the same element to itself
//                db("2.5");
//                char *toComp = splitData[j];
//                db("3");
//                dbi(j);
//                // db(toComp);
//                db(chosen);
//                if (strcmp(toComp, chosen) == 0) {
//                    db("4");
//                    splitLen = splitLen - 1;
//                    j = j - 1;
//                    db("5");
//                    for (int k = i; k < splitLen; k++) {
//                        db("6");
//                        splitData[k] = splitData[k + 1];
//                    }
//                }
//            }
//        }
    }

    for (int i = 0; i < splitLen; i++) printf("%s", splitData[i]);
}


char **deleteThis(char **str, char *elem, int size) {
    int count;
    for (count = 0; count < size; count++) {
        if (strcmp(elem, str[count]) == 0) break;
    }

    if (count < size) {
        size = size - 1;
        for (int i = 0; i < size; i++) str[i] = str[i + 1];
    }
    return str;

}


char **splitFile(char *input) {
    char *splitStr = strtok(input, "\n");
    char **lines = malloc(maxLines * (sizeof(char *)));
    int lineCount = 0;
    while (splitStr != NULL) {
        if (lineCount > maxLines) break;
        lines[lineCount] = malloc(maxCharsPerLine * (sizeof(char)));
        lines[lineCount] = splitStr;
        splitStr = strtok(NULL, "\n");
        lineCount++;
    }
    return lines;
}