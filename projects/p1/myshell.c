/*
    Maggie Horton
    CS-451
    Summer 2020
*/
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define cmdlen 20
#define errflag -1
#define false 0
#define maxinfo 5
#define maxlen 70
#define true 1

const char *pipeFlag = "|";
const char *quit = "quit\n";

int comp = 1;

// function signatures
char *clean(char *str);
char **splitFile(char *input);
int found(char *filePath);
int elemCount(char **arr);
void handleInput(char *input);
void pipeHandler(char *cmdOne, char *cmdTwo);
void runCmd(char *initCmd, char *txtPath);


int main(int argc, char **argv) {
    char input[maxlen];
    printf("Hello and welcome to my humble, simple shell. It can run 3 commands: my-cat, my-uniq & my-wc.\n"
           "These commands are intended to run similarly to their linux  counterparts except without all "
           "of the fancy flags or man page entries.\nSupply a text file name or path to any of the commands, or "
           "if you're feeling wild, feel free to do some piping (as long as you're using the supplied commands.)\n");
    while (comp != 0) {
        printf("\n\nmyprompt> ");
        fgets(input, maxlen, stdin);
        comp = strcmp(quit, input);
        if (comp == 0) {
            printf("\n\nSee ya!\n\n");
            kill(0, SIGINT);
            return 0;
        } else handleInput(input);
    }
    return 0;
}


void handleInput(char *input) {
    if (strcmp(input, "\n") != 0 && (input != NULL)) { // make sure theres some input
        char *space = strchr(input, ' ');
        if (space == NULL)
            printf("incorrect arg format or unrecognized command, try again\n"); // avoid segfault w/ strtok
        else { // need to handle as much as cmd file | cmd | cmd
            char **split = splitFile(input);
            int items = elemCount(split);

            char *initCmd = clean(split[0]);
            if (found(initCmd) == errflag) printf("\ncan't find file %s\n", initCmd);
            char *txtPath = clean(split[1]);

            if (items == 2) { // basic command
                runCmd(initCmd, txtPath);
            } else if ((items == 4) || (items == 6)) { // single pipe
                char *pipe1 = clean(split[2]);
                char *cmd2 = clean(split[3]);
                if (items == 6) {
                    char *pipe2 = clean(split[4]);
                    char *cmd3 = clean(split[5]);
                    runCmd(initCmd, txtPath);
                } else {
                    runCmd(initCmd, txtPath);
                }

            } else if ((items == 3) || (items == 5)) { // incorrect no. of items supplied
                printf("wrong number of items supplied\n");
            }
        }
    } else {
        printf("invalid input\n");
        return;
    }
}


void runCmd(char *initCmd, char *txtPath) {
    __pid_t forked = fork();
    char run[maxlen + 3] = "./";
    strcat(run, initCmd);

    if (forked == 0) {
        char *args[] = {run, txtPath, NULL};
        execvp(args[0], args);
        printf("\n");
    } else waitpid(forked, 0, 0);
}


void pipeHandler(char *cmdOne, char *cmdTwo) {

}


int found(char *filePath) {
    if (access(filePath, R_OK) != errflag) return true;
    else return errflag;
}


int elemCount(char **arr) {
    int i = 0;
    for (; i < maxinfo; i++) {
        if ((arr[i] == NULL) || strcmp(arr[i], "\n") == 0 || strcmp(arr[i], " ") == 0) break;
    }
    return i;
}


char **splitFile(char *input) {
    char *splitStr = strtok(input, " ");
    char **runInfo = malloc(maxinfo * (sizeof(char *)));
    int cmdCount = 0;
    while (splitStr != NULL) {
        if (cmdCount > maxinfo) break;
        runInfo[cmdCount] = malloc(cmdlen * (sizeof(char)));
        runInfo[cmdCount] = splitStr;
        splitStr = strtok(NULL, " ");
        cmdCount++;
    }
    return runInfo;
}


char *clean(char *str) {
    char *stripped = malloc(strlen(str));
    int new = 0, old = 0;
    for (; old < strlen(str); old++, new++) {
        if (!isspace(str[old])) stripped[new] = str[old];
        else new--;
    }
    stripped[new] = 0;
    return stripped;
}