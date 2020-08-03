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

// used as a 'flag', very unlikely file name
#define NO_PATH "#%?no#%?path#%?"
#define TAG  "my-cat:"

void cat(char *f_name);


/******************************************************************************
 * Function: main
 * Details: This is the main function for the my-cat command which just
 *          checks for a file path in args. If no path is found, then it
 *          assumes that it is being passed data from a pipe, then a flag-string
 *          is passed to my-cat() so that it knows to look for data from stdin.
 * Input: Command/program name (char*) & input text file path (char*) supplied
 *        from the commandline.
 * Output: 0 upon successful execution
 ******************************************************************************/
int main(int argc, char **argv) {
    char *inp_path;
    if (argc == 1) inp_path = NO_PATH;
    else inp_path = argv[1];
    cat(inp_path);
}


/******************************************************************************
 * Function: cat
 * Details: Replicates the base functionality of the linux command cat. Reads
 *          a file & displays its contents in the terminal.
 * Input: Path of file to display (char*)
 * Output: Nothing
 ******************************************************************************/
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