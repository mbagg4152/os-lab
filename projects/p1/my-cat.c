/*
    Maggie Horton
    CS-451
    Summer 2020
    Project 1: my-cat.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define NO_PATH "#%?no#%?path#%?"
#define TAG  "my-cat:"

void cat(char *f_name);


int main(int argc, char **argv) {
    char *inp_path;
    if (argc == 1) inp_path = NO_PATH;
    else inp_path = argv[1];
    cat(inp_path);
}


void cat(char *f_name) {
    if (strcmp(f_name, NO_PATH) == 0) {
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
        FILE *txt = fopen(f_name, "r");
        if (txt == NULL) { // required err message & action on file access failure
            printf("%s cannot open file '%s'.\n", TAG, f_name);
            exit(1);
        }

        char temp;
        while (!feof(txt)) {
            fscanf(txt, "%c", &temp);
            fputc(temp, stdout);
        }
    }

}