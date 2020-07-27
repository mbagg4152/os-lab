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
#include <signal.h>

#define cmdlen 20
#define errflag -1
#define maxinfo 5   
#define maxlen 70
#define false 0
#define true 1

const char* quit = "quit\n";
const char* pflag = "|";

// consts for output only
const char* dn = "\n\n";
const char* tn = "\n\n\n";
const char* qn = "\n\n\n\n";

int comp = 1;

// function signatures
char** splitInput(char* input);
int found(char* fpath);
int valCount(char** arr); 
void pipeHandler(char* cmdOne, char* cmdTwo);
int runCmd(char* initCmd);

int main(int argc, char** argv){
    char input[maxlen];
    while(comp != 0){
        printf("%smyprompt> ", dn);
        fgets(input, maxlen, stdin);
        comp = strcmp(quit, input);
        if(comp == 0) {
            printf("%sSee ya!%s", dn, dn);
            return 0;
            //kill(0, SIGKILL);
        }
        if((input != "\n") && (input != NULL)){ // make sure theres some imput
            char* space = strchr(input, ' ');
            if(space == NULL) printf("incorrent arg format, try again\n"); // avoid segfault w/ strtok
            else { // need to handle as much as cmd file | cmd | cmd
                char** split = splitInput(input);
                int slen = valCount(split);
                printf("vals arr size: %d", slen);
                char* initCmd = split[0];

                



                // printf("%s",initCmd);
                runCmd(initCmd);
            }
        





            // char* cname = strtok(input, " "); // get name of program to run
            // char* fname = strtok(NULL, " "); // get argument for program
            // int fnameLen = strlen(fname);
            // fname[fnameLen - 1] = '\0'; // just gotta make sure that extra spaces are removed or else it doesnt work right
        
            // // maybe instead of tiny little function found, just do what the function does here
            // if(found(cname) == bad) printf("%scan't find specified file %s, try again%s", dn, cname, dn);
            // else if(found(fname) == bad) printf("%scan't find specified file %s, try again%s", dn, fname, dn);
            // else {
            //     printf("%s", dn);
            //     runSort(cname,fname);  // tbh could probably just move the stuff from runSort here
            //     printf("%s", dn);
            // }
            }

       
    }
    return 0;
}

int runCmd(char* initCmd){
    __pid_t pid = fork();
    char run[maxlen + 3] = "./";
    strcat(run,initCmd);
    if(pid == 0){
        char* args[] = {run, NULL};
        execvp(args[0], args);
        printf("\n");
        
    } else {
        waitpid(pid, 0, 0);
        kill(0, SIGINT); // kill the child
    }
    
    return 0;
}

void pipeHandler(char* cmdOne, char* cmdTwo){

}

int found(char* fpath){
    if(access(fpath, F_OK) != errflag) return true;
    else return errflag;
}

int valCount(char** arr){
    int i;
    for (i; i < maxinfo; i++) if((arr[i] == NULL) || (arr[i] == "\n") || (arr[i] == " ")) break;
    return i;
}

char** splitInput(char* input){
    //printf("sent to split: %s\n", input);
    char* tok = strtok(input, " ");
    char **runInfo = malloc(maxinfo * (sizeof(char*)));
    int cmds;
    while(tok != NULL){
        if(cmds > maxinfo) break;
        if(tok == " \n") cmds++;
        else {
            runInfo[cmds] = malloc(cmdlen * (sizeof(char)));
            runInfo[cmds] = tok;
            tok = strtok(NULL, " ");
            cmds++;
        }
    }

    // printf("vals input: ");
    // for(int i = 0; i < (cmds + 1); i++) printf("%s ", runInfo[i]);
    // printf("%s", dn);
    
    return runInfo;
}