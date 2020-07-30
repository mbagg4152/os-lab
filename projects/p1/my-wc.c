#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void wc(char* txtPath);

int cap = 20;

int main(int argc, char** argv){
    if(argc == 1) printf("no filename supplied as an argument.");
    else {
        char* fname = argv[1];
        wc(fname);
    }
    return 0;
}


void wc(char* txtPath){
    FILE* txt = fopen(txtPath, "r");
    if(txt == NULL) {
        printf("either file %s does not exist or it cannot be accessed.\n", txtPath);
        return;
    } else {
        int counter = 0, words = 0, lines = 0;
        fseek(txt, 0, SEEK_END);
        long txtSize = ftell(txt);
        rewind(txt);
        char* data = malloc(txtSize + 1);
        fread(data, 1, txtSize, txt);
        fclose(txt);
 
        if(isalnum(data[0])) words++;
        for(counter; counter < txtSize; counter++){
            char cur = data[counter];
            //printf("%c", cur);
            if(isspace(cur)){
                if(counter < txtSize - 1){
                    char next = data[counter + 1];
                    if(isalnum(next)){
                        if(cur == '\n'){
                            words++;
                            lines++;
                        } else if((cur == ' ') || (cur == '\t')) words++;
                    } else if (cur == '\n') lines++;
                }
            }

        }



        printf("\nFor file '%s': lines = %d, words = %d & bytes = %d\n\n", txtPath, lines, words, counter);


    }
}
