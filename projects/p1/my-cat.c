#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void cat(char* fname);

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("no file name supplied\n");
        return 0;
    }
    else {
        cat(argv[1]);
        return 0;
    }
}

void cat(char* fname){
    FILE* txt = fopen(fname, "r");
    if(txt == NULL) {
        printf("either file %s does not exist or it cannot be accessed.\n", fname);
        return;
    }
    
    char temp;
    while (!feof(txt)) {
        fscanf(txt, "%c", &temp);
        printf("%c", temp);
    }
    printf("\n");
}