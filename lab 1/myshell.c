#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXIN 50

const char* key = "quit\n";

void runSort(char* cname, char* fname);




int main(int argc, char** argv){
    char input[MAXIN];
    int comp = 1;

    while(comp != 0){
        printf("myprompt> ");
        fgets(input, MAXIN, stdin);
        comp = strcmp(key, input);

        char progArgs[2][50];
        int inner, count;
        for (int i = 0; i >= strlen(input); i++){
            if (input[i] == ' '){
                progArgs[count][inner] == '\0';
                count++;
                inner = 0;
            }
            else {
                progArgs[count][inner] = input[i];
                inner++;
            }
        }



        runSort(progArgs[0],progArgs[1]);        
        printf("%s, %s\n", progArgs[0],progArgs[1]);
    }

    return 0;
}

void runSort(char* cname, char* fname){
    __pid_t pid = fork();
    char runner[55] = "./";
    strcat(runner, cname);
    if (pid == 0){
        char* argv[] = {runner, fname, NULL};
        execv(argv[0], argv);
    }
    else waitpid(pid,0,0);
}