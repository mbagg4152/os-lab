/*
    Maggie Horton
    CS-451
    Summer 2020
    Project 1: my-wc.c
*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void wc(char *path);

// used as a 'flag', very unlikely file name
#define NO_PATH "#%?no#%?path#%?"
#define TAG  "my-wc:"
int cap = 512;


/******************************************************************************
 * Function: main
 * Details: This is the main function for the my-wc command which just
 *          checks for a file path in args. If no path is found, then it
 *          assumes that it is being passed data from a pipe, then a flag-string
 *          is passed to wc() so that it knows to look for data from stdin.
 * Input: Command/program name (char*) & input text file path (char*) supplied
 *        from the commandline.
 * Output: 0 upon successful execution
 ******************************************************************************/
int main(int argc, char **argv) {
    char *txtFileName;
    if (argc == 1) txtFileName = NO_PATH;
    else txtFileName = argv[1];
    wc(txtFileName);
    return 0;
}


/******************************************************************************
 * Function: wc
 * Details: Replicates the base functionality of the linux command wc. Takes in
 *          a file & outputs the number of lines & words in the file.
 * Input: Path to file (char*)
 * Output: Nothing
 ******************************************************************************/
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
        }
    } else {
        FILE *txt = fopen(path, "r");
        fseek(txt, 0, SEEK_END);
        txtSize = ftell(txt);
        rewind(txt);
        data = malloc(txtSize);
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

    fprintf(stdout, "\n%s Lines = %d & words = %d\n", TAG, lines, words);

}
