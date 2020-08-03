/*
    Maggie Horton
    CS-451
    Summer 2020
    Project 1: my-uniq.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_CHARS 1024
#define MAX_LINES 400
#define TOTAL_MAX  MAX_CHARS * MAX_LINES
// used as a 'flag', very unlikely file name
#define NO_PATH "#%?no#%?path#%?"
#define TAG  "my-my_uniq"

void my_uniq(char *inp_path);


/******************************************************************************
 * Function: main
 * Details: This is the main function for the my-uniq command which just
 *          checks for a file path in args. If no path is found, then it
 *          assumes that it is being passed data from a pipe, then a flag-string
 *          is passed to my_uniq() so that it knows to look for data from stdin.
 * Input: Command/program name (char*) & input text file path (char*) supplied
 *        from the commandline.
 * Output: 0 upon successful execution
 ******************************************************************************/
int main(int argc, char **argv) {
    char *inp_path;
    if (argc == 1) inp_path = NO_PATH;
    else inp_path = argv[1];
    my_uniq(inp_path);
    return 0;
}


/******************************************************************************
 * Function: my_uniq
 * Details: Replicates the base functionality of the linux command uniq. Reads
 *          in a file. If any duplicate lines are found then they are removed.
 * Input: Path to text file to be read  (char*)
 * Output: Nothing
 ******************************************************************************/
void my_uniq(char *inp_path) {
    char inp_lines[MAX_LINES][MAX_CHARS];
    FILE *file_to_check;
    if (strcmp(inp_path, NO_PATH) == 0) { file_to_check = stdin; }
    else file_to_check = fopen(inp_path, "r");

    int count = 0;
    while (fgets(inp_lines[count], MAX_CHARS, file_to_check)) {
        inp_lines[count][strlen(inp_lines[count]) - 1] = '\n';
        count++;
    }

    int len = count;
    for (int i = 0; i < len; i++) {
        for (int j = i + 1; j < len;) {
            if (strcmp(inp_lines[j], inp_lines[i]) == 0) {
                for (int k = j; k < len; k++) {
                    strcpy(inp_lines[k], inp_lines[k + 1]);
                }
                len--;
            } else {
                j++;
            }
        }
    }
    for (int i = 0; i < len; i++) printf("%s", inp_lines[i]);
}