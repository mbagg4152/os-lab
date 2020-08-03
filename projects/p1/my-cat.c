#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define NO_PATH "#%?no#%?path#%?"
#define TAG  "my-cat:"

void cat(char *fname);


int main(int argc, char **argv) {
    char *txtPath;
    if (argc == 1) txtPath = NO_PATH;
    else txtPath = argv[1];
    cat(txtPath);
    return 0;
}


void cat(char *fname) {
    if (strcmp(fname, NO_PATH) == 0) {
        int c = fgetc(stdin);
        if (stdin != NULL) {
            while (c != EOF) {
                putchar(c);
                c = fgetc(stdin);
            }
        } else {
            printf("%s There was no file name supplied & stdin is empty. Nothing to do here", TAG);
            exit(1);
        }

    } else {
        FILE *txt = fopen(fname, "r");
        if (txt == NULL) { // required err message & action on file access failure
            printf("%s cannot open file '%s'.\n", TAG, fname);
            exit(1);
        }

        char temp;
        while (!feof(txt)) {
            fscanf(txt, "%c", &temp);
            fputc(temp, stdout);
        }
    }

}