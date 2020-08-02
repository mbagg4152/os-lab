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

#define CMD_LEN 20
#define ERR_FLG -1
#define false 0
#define MAX_ARGS 5
#define MAX_LEN 70
#define true 1
#define SINGLE_COUNT 4
#define DOUBLE_COUNT 5
#define READ 0 // == STDIN_FILENO
#define WRITE 1 // == STDOUT_FILENO

const char *pipeFlag = "|";
const char *quit = "quit\n";
const char *tag = "myshell:";

int comp = 1;

// function signatures
char *cleanString(char *str);
char **splitFile(char *input);
int foundFile(char *filePath);
int elemCount(char **arr);
void handleInput(char *input);
void pipeHandler(char *cmdArgs[], int argCount);
void runCmd(char *initCmd, char *txtPath);


int main(int argc, char **argv) {
    char input[MAX_LEN];
    printf("Hello and welcome to my humble, simple shell. It can run 3 commands: my-cat, my-uniq & my-wc.\n"
           "These commands are intended to run similarly to their linux  counterparts except without all "
           "of the fancy flags or man page entries.\nSupply a text file name or path to any of the commands, or "
           "if you're feeling wild, feel free to do some piping (as long as you're using the supplied commands.)\n");
    while (comp != 0) {
        printf("\n\nmyprompt> ");
        fgets(input, MAX_LEN, stdin);
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
            printf("%s incorrect arg format or unrecognized command, try again\n", tag); // avoid segfault w/ strtok
        else { // need to handle as much as cmd file | cmd | cmd
            char **split = splitFile(input);
            int items = elemCount(split);

            char *initCmd = cleanString(split[0]);
            if (foundFile(initCmd) == ERR_FLG) printf("\n%s oh no can't find file %s\n", tag, initCmd);
            else {
                char *txtPath = cleanString(split[1]);

                if (items == 2) { // basic command
                    runCmd(initCmd, txtPath);
                } else if ((items == 3) || (items == 5)) printf("%s wrong number of items supplied\n", tag);
                else { // single pipe
                    char *firstPipe = cleanString(split[2]), *secondCmd = cleanString(split[3]);
                    char *singlePipeArgs[] = {initCmd, txtPath, secondCmd, NULL};

                    if (items == 6) {
                        char *secondPipe = cleanString(split[4]), *thirdCmd = cleanString(split[5]);
                        char *doublePipeArgs[] = {initCmd, txtPath, secondCmd, thirdCmd, NULL};

                        if (strcmp(secondPipe, pipeFlag) != 0)
                            printf("%s expected a pipe but got '%s'\n", tag, secondPipe);
                        else {
                            if (foundFile(thirdCmd) == ERR_FLG) printf("\n%s can't find file %s\n", tag, thirdCmd);
                            else pipeHandler(doublePipeArgs, DOUBLE_COUNT);
                        }
                    }

                    if (strcmp(firstPipe, pipeFlag) != 0)
                        printf("%s expected a pipe but got '%s'\n", tag, firstPipe);
                    else {
                        if (foundFile(secondCmd) == ERR_FLG) printf("\n%s can't find file %s\n", tag, secondCmd);
                        //else runCmd(initCmd, txtPath);
                    }
                    if (items == 4) {
                        pipeHandler(singlePipeArgs, SINGLE_COUNT);
                    }

                }

            }
        }
    }

}


void runCmd(char *initCmd, char *txtPath) {
    __pid_t forked = fork();
    char run[MAX_LEN + 3] = "./";
    strcat(run, initCmd);

    if (forked == 0) {
        char *args[] = {run, txtPath, NULL};
        execvp(args[0], args);
        printf("\n");
    } else if (forked < 0) {
        printf("%s encountered error while forking", tag);
    } else waitpid(forked, 0, 0);
}


void pipeHandler(char *cmdArgs[], int argCount) {
    int singlePids[2];
    int doublePids[3];
    int first[2]; // store ends of pipe from cmd1 -> cmd2
    int second[2]; // store ends of pipe from cmd2 -> cmd3
    char *firstArgs[] = {cmdArgs[0], cmdArgs[1], NULL};
    char *secondArgs[] = {cmdArgs[2], NULL};
    pid_t fPid = -1;
    pid_t sPid = -1;
    if (argCount == SINGLE_COUNT) {
        // whatever is written from descriptor[1] is read from descriptor[0]
        int desc[2];

        int piped = pipe(desc);
        if (piped < 0) printf("%s error making pipe\n", tag);
        else {
            int forked = fork();
            int status;

            // idea: redirect stdout to the pipe & close read end in the parent, then execute first in parent
            // replace stdin with input from pipe, which contains output from first process. close write end of pipe since it will not be used then run second command
            if (forked < 0) printf("%s err forking\n", tag);

            if (forked == 0) {
                close(desc[READ]);
                dup2(desc[WRITE], WRITE);
                execv(firstArgs[0], firstArgs);
            }
            close(desc[WRITE]);
            waitpid(-1, &status, 0);

            int forked2 = fork();
            if (forked2 == 0) {
                close(desc[WRITE]);
                dup2(desc[READ], READ);
                execv(secondArgs[0], secondArgs);
            }

            close(desc[READ]);

            waitpid(-1, &status, 0);
        }

    } else if (argCount == DOUBLE_COUNT) {
        printf("%s bleh\n", tag);
    } else {
        printf("%s blah\n", tag);
    }
}


int foundFile(char *filePath) {
    if (access(filePath, R_OK) != ERR_FLG) return true;
    else return ERR_FLG;
}


int elemCount(char **arr) {
    int i = 0;
    for (; i < MAX_ARGS; i++) {
        if ((arr[i] == NULL) || strcmp(arr[i], "\n") == 0 || strcmp(arr[i], " ") == 0) break;
    }
    return i;
}


char **splitFile(char *input) {
    char *splitStr = strtok(input, " ");
    char **runInfo = malloc(MAX_ARGS * (sizeof(char *)));
    int cmdCount = 0;
    while (splitStr != NULL) {
        if (cmdCount > MAX_ARGS) break;
        runInfo[cmdCount] = malloc(CMD_LEN * (sizeof(char)));
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