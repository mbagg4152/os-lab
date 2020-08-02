#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

const char *tag = "my-cat:";
void cat(char *fname);


int main(int argc, char **argv) {
    if (argc == 1) {
        printf("%s a filename must be supplied\n", tag);
        return 0;
    } else {
        cat(argv[1]);
        return 0;
    }
}


void cat(char *fname) {
    FILE *txt = fopen(fname, "r");
    if (txt == NULL) { // required err message & action on file access failure
        printf("%s cannot open file '%s'.\n", tag, fname);
        exit(1);
    }

    char temp;
    while (!feof(txt)) {
        fscanf(txt, "%c", &temp);
        fputc(temp, stdout);
    }
    printf("\n");
}