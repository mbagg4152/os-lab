/*
    Maggie Horton
    CS-451
    Summer 2020
    myshell.c: takes input to either run mysort.c or quit
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define maxlen 50
#define good 2
#define bad -1

const char* key = "quit\n";
const char* dnl = "\n\n";
const char* fnl = "\n\n\n\n";

void runSort(char* cname, char* fname);
int exists(char* fpath); 

int main(int argc, char** argv){
    char input[maxlen];
    int comp = 1;
    while(comp != 0){
        printf("myprompt> ");
        fgets(input, maxlen, stdin);
        comp = strcmp(key, input);
        if(comp == 0) {
            printf("%sSee ya!%s", fnl, dnl);
            break;
        }
        char* space = strchr(input, ' ');
        if(space == NULL) printf("incorrent arg format, try again\n"); // avoid segfault w/ strtok
        else {
            char* cname = strtok(input, " "); // get name of program to run
            char* fname = strtok(NULL, " "); // get argument for program
            int fnameLen = strlen(fname);
            fname[fnameLen - 1] = '\0';
        
            if(exists(cname) == bad) printf("%scan't find specified file %s, try again%s", dnl, cname, dnl);
            else if(exists(fname) == bad) printf("%scan't find specified file %s, try again%s", dnl, fname, dnl);
            else {
                printf("%s", dnl);
                runSort(cname,fname);
                printf("%s", dnl);
            }
        }
    }
    return 0;
}

void runSort(char* cname, char* fname){
    __pid_t pid = fork();
    char runArg[maxlen + 3] = "./";
    strcat(runArg, cname);


    if (pid == 0){
        char* args[] = {runArg, fname, NULL};
        execvp(args[0], args);
    }   else waitpid(pid,0,0);
}

int exists(char* fpath){
    if(access(fpath, F_OK) != bad) return good;
    else return bad;
}

