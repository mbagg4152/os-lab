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

#define cmdLen 20
#define errFlag -1
#define false 0
#define maxArgs 5
#define maxLen 70
#define true 1

const char *pipeFlag = "|";
const char *quit = "quit\n";

int comp = 1;

// function signatures
char *cleanString(char *str);
char **splitFile(char *input);
int foundFile(char *filePath);
int elemCount(char **arr);
void handleInput(char *input);
void pipeHandler(char *cmdOne, char *cmdTwo);
void runCmd(char *initCmd, char *txtPath);


int main(int argc, char **argv) {
    char input[maxLen];
    printf("Hello and welcome to my humble, simple shell. It can run 3 commands: my-cat, my-uniq & my-wc.\n"
           "These commands are intended to run similarly to their linux  counterparts except without all "
           "of the fancy flags or man page entries.\nSupply a text file name or path to any of the commands, or "
           "if you're feeling wild, feel free to do some piping (as long as you're using the supplied commands.)\n");
    while (comp != 0) {
        printf("\n\nmyprompt> ");
        fgets(input, maxLen, stdin);
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

            char *initCmd = cleanString(split[0]);
            if (foundFile(initCmd) == errFlag) printf("\ncan't find file %s\n", initCmd);
            char *txtPath = cleanString(split[1]);

            if (items == 2) { // basic command
                runCmd(initCmd, txtPath);
            } else if ((items == 4) || (items == 6)) { // single pipe
                char *pipe1 = cleanString(split[2]);
                char *cmd2 = cleanString(split[3]);
                if (strcmp(pipe1, pipeFlag) == 0) {
                    runCmd(initCmd, txtPath);
                } else {
                    printf("myshell: expected a pipe but got '%s'\n", pipe1);
                }

                if (items == 6) {
                    char *pipe2 = cleanString(split[4]);
                    if (strcmp(pipe2, pipeFlag) == 0) {
                        runCmd(initCmd, txtPath);

                    } else {
                        printf("myshell: expected a pipe but got '%s'\n", pipe2);
                    }

                    char *cmd3 = cleanString(split[5]);

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
    char run[maxLen + 3] = "./";
    strcat(run, initCmd);

    if (forked == 0) {
        char *args[] = {run, txtPath, NULL};
        execvp(args[0], args);
        printf("\n");
    } else waitpid(forked, 0, 0);
}


void pipeHandler(char *cmdOne, char *cmdTwo) {

}


int foundFile(char *filePath) {
    if (access(filePath, R_OK) != errFlag) return true;
    else return errFlag;
}


int elemCount(char **arr) {
    int i = 0;
    for (; i < maxArgs; i++) {
        if ((arr[i] == NULL) || strcmp(arr[i], "\n") == 0 || strcmp(arr[i], " ") == 0) break;
    }
    return i;
}


char **splitFile(char *input) {
    char *splitStr = strtok(input, " ");
    char **runInfo = malloc(maxArgs * (sizeof(char *)));
    int cmdCount = 0;
    while (splitStr != NULL) {
        if (cmdCount > maxArgs) break;
        runInfo[cmdCount] = malloc(cmdLen * (sizeof(char)));
        runInfo[cmdCount] = splitStr;
        splitStr = strtok(NULL, " ");
        cmdCount++;
    }
    return runInfo;
}


char *cleanString(char *str) {
    char *stripped = malloc(strlen(str));
    int new = 0, old = 0;
    for (; old < strlen(str); old++, new++) {
        if (!isspace(str[old])) stripped[new] = str[old];
        else new--;
    }
    stripped[new] = 0;
    return stripped;
}