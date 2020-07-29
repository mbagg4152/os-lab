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
int itemCount(char** arr); 
void handleInput(char* input);
void pipeHandler(char* cmdOne, char* cmdTwo);
void runCmd(char* initCmd, char* txtPath);


int main(int argc, char** argv){
    char input[maxlen];
    printf("Hello and welcome to my humble, simple shell. It can run 3 commands: my-cat, my-uniq & my-wc.\n"
           "These commands are intended to run similarly to their linux  counterparts except\nwithout all"
           " of the fancy flags or man page entries.\nSupply a text file name or path to any of the commands, or "
           "if you're feeling wild,\nfeel free to do some piping (as long as you're using the supplied commands.)\n");
    while(comp != 0){
        printf("%smyprompt> ", dn);
        fgets(input, maxlen, stdin);
        comp = strcmp(quit, input);
        if(comp == 0) {
            printf("%sSee ya!%s", dn, dn);
            kill(0, SIGINT);
            return 0;
        } else handleInput(input);
    }
    return 0;
}


void handleInput(char* input){
    if((input != "\n") && (input != NULL)){ // make sure theres some imput
        char* space = strchr(input, ' ');
        if(space == NULL) printf("incorrent arg format, try again\n"); // avoid segfault w/ strtok
        else { // need to handle as much as cmd file | cmd | cmd
            char** split = splitInput(input);
            int items = itemCount(split);

            char* initCmd = split[0];
            if (found(initCmd) == errflag) printf("\ncan't find file %s\n", initCmd);
                char* txtPath = split[1];
                txtPath[(strlen(txtPath)) - 1] = '\0'; // make sure there's no weird chars @ the end

                if(items == 2) { // basic command
                    runCmd(initCmd, txtPath);
                } else if((items == 3) || (items == 5)) { // incorrect no. of items supplied
                    printf("wrong number of items supplied\n");
                } else if(items == 4) { // single pipe 
                    runCmd(initCmd, txtPath);
                } else if(items == 6) { // double pipe
                    runCmd(initCmd, txtPath);
                }   
        }             
    } else {
        printf("invalid input\n");
        return;
    }
}


void runCmd(char* initCmd, char* txtPath){
    __pid_t pid = fork();
    char run[maxlen + 3] = "./";
    strcat(run,initCmd);


    if(pid == 0){
        char* args[] = {run, txtPath, NULL};
        execvp(args[0], args);
        printf("\n");
    } else {
        waitpid(pid, 0, 0);
    }
    
    return;
}

void pipeHandler(char* cmdOne, char* cmdTwo){

}

int found(char* fpath){
    if(access(fpath, R_OK) != errflag) return true;
    else return errflag;
}

int itemCount(char** arr){
    int i;
    for (i; i < maxinfo; i++) if((arr[i] == NULL) || (arr[i] == "\n") || (arr[i] == " ")) break;
    return i;
}

char** splitInput(char* input) {
    char* splitStr = strtok(input, " ");
    char **runInfo = malloc(maxinfo * (sizeof(char*)));
    int cmdCount;
    while(splitStr != NULL){
        if(cmdCount > maxinfo) break;
        if(splitStr == " \n") cmdCount++;
        else {
            runInfo[cmdCount] = malloc(cmdlen * (sizeof(char)));
            runInfo[cmdCount] = splitStr;
            splitStr = strtok(NULL, " ");
            cmdCount++;
        }
    }
    return runInfo;
}