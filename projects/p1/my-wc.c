#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void wc(char *path);

#define NO_PATH "#%?no#%?path#%?"

int cap = 512;
const char *TAG = "my-wc:";


int main(int argc, char **argv) {
    char *txtFileName;
    if (argc == 1) txtFileName = NO_PATH;
    else txtFileName = argv[1];

    wc(txtFileName);
    return 0;
}


void wc(char *path) {
    char *data;
    int counter = 0, words = 0, lines = 0;
    long txtSize;
    if (strcmp(path, NO_PATH) == 0) {  // required err message & action on file access failure

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
        txtSize = used;
        if (txtSize == 0) {
            printf("%s There was no file name supplied & stdin is empty. Nothing to do here\n", TAG);
            exit(1);
        } else printf("%s Found some data in stdin, will use this for the wc operation.\n", TAG);
    } else {
        FILE *txt = fopen(path, "r");
        fseek(txt, 0, SEEK_END);
        txtSize = ftell(txt);
        rewind(txt);
        data = malloc(txtSize + 1);
        fread(data, 1, txtSize, txt);
        fclose(txt);
    }

    if (isalnum(data[0])) words++;
    for (; counter < txtSize; counter++) {
        char cur = data[counter];
        if (isspace(cur)) {
            if (counter < txtSize - 1) {
                char next = data[counter + 1];
                if (isalnum(next)) {
                    if (cur == '\n') {
                        words++;
                        lines++;
                    } else if ((cur == ' ') || (cur == '\t')) words++;
                } else if (cur == '\n') lines++;
            }
        }

    }
    printf("\n%s Lines = %d, words = %d & bytes = %d\n", TAG, lines, words, counter);

}
