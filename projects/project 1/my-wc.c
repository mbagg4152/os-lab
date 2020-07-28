#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void wc(char* txtPath);

int cap = 20;

int main(int argc, char** argv){
    if(argc == 1) printf("no filename supplied as an argument.");
    else {
        
        char* fname = argv[1];
        printf("filename %s supplied\n", fname);
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
        int counter, words, lines;
        char temp;

        for (temp = getc(txt); temp != EOF; temp = getc(txt)){
            
            if(temp == '\n') lines++;
            if((temp == ' ')) words++;
            counter++;
        }
        fclose(txt);

        printf("\n  %d  %d  %d  %s\n", lines, words, counter, txtPath);


    }
}
