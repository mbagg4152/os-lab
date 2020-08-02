#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void wc(char *path);

int cap = 20;
const char *tag = "my-wc:";


int main(int argc, char **argv) {
    if (argc == 1) {
        printf("%s no filename supplied as an argument. trying stdin\n", tag);
        char *txtFileName = STDIN_FILENO;
        wc(txtFileName);
    } else {
        char *txtFileName = argv[1];
        wc(txtFileName);
    }
    return 0;
}


void wc(char *path) {
    FILE *txt = fopen(path, "r");
    if (txt == NULL) {  // required err message & action on file access failure
        printf("%s cannot open file '%s'.\n", tag, path);
        txt = stdin;
        //exit(1);
    }
    int counter = 0, words = 0, lines = 0;
    fseek(txt, 0, SEEK_END);
    long txtSize = ftell(txt);
    rewind(txt);
    char *data = malloc(txtSize + 1);
    fread(data, 1, txtSize, txt);
    fclose(txt);

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
    printf("\n%s For file '%s': lines = %d, words = %d & bytes = %d\n\n", tag, path, lines, words, counter);
//    char lineStr[10], wordStr[10], byteStr[12], out[100];
//    char *str1 = " For file '", *str2 = "': lines = ", *str3 = ", words = ", *str4 = " & bytes = ", *str5 = "\n";
//    sprintf(lineStr, "%d", lines);
//    sprintf(wordStr, "%d", words);
//    sprintf(byteStr, "%d", counter);
//    sprintf(out, "%s%s%s%s%s%s%s%s%s%s", tag, str1, path, str2, lineStr, str3, wordStr, str4, byteStr, str5);
//    for (int i = 0; i < strlen(out); i++) fputc(out[i], stdout);

}
