/*
    Maggie Horton
    CS-451
    Summer 2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define errflag -1  
#define maxlen 70
#define false 0
#define true 1

const char* key = "quit\n";
const char* dnl = "\n\n";
const char* fnl = "\n\n\n\n";

void runCmd(char* cmdName);
int found(char* fpath); 

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
        
        // all the string stuff could probably be moved into different function
        char* space = strchr(input, ' '); // i dont think theres a check for invalid input so this may not be necessary
        //if(space == NULL) printf("incorrent arg format, try again\n"); // avoid segfault w/ strtok
        // else {

            char* cmdName = strtok(input, " ");
            printf("%s",cmdName); 
            // char* cname = strtok(input, " "); // get name of program to run
            // char* fname = strtok(NULL, " "); // get argument for program
            // int fnameLen = strlen(fname);
            // fname[fnameLen - 1] = '\0'; // just gotta make sure that extra spaces are removed or else it doesnt work right
        
            // // maybe instead of tiny little function found, just do what the function does here
            // if(found(cname) == bad) printf("%scan't find specified file %s, try again%s", dnl, cname, dnl);
            // else if(found(fname) == bad) printf("%scan't find specified file %s, try again%s", dnl, fname, dnl);
            // else {
            //     printf("%s", dnl);
            //     runSort(cname,fname);  // tbh could probably just move the stuff from runSort here
            //     printf("%s", dnl);
            // }
        // }
    }
    return 0;
}


void runCmd(char* cmdName){
    __pid_t pid = fork();
    char run[maxlen + 3] = "./";
    strcat(run,cmdName);
    if(pid == 0){
        char* args[] = {run, NULL};
        execvp(args[0], args);
    } else waitpid(pid,0,0);
}

int found(char* fpath){
    if(access(fpath, F_OK) != errflag) return true;
    else return errflag;
}

