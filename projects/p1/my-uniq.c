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
#define TAG  "my-my_uniq"

void my_uniq(char *inp_path);


int main(int argc, char **argv) {
    char *inp_path;
    if (argc == 1) inp_path = NO_PATH;
    else inp_path = argv[1];
    my_uniq(inp_path);
    return 0;
}


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