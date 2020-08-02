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

// arbitrary length limits
#define CMD_LEN 48          // size limit per command
#define MAX_ARG_LEN 5       // max length of array holding args
#define MAX_INP_LEN 240     // max length for input

// defs to determine how to handle pipes
#define NO_PIPE 2           // number of expected args for simple command
#define SINGLE_PIPE 4       // number of args expected for single piping
#define DOUBLE_PIPE 6       // number of args expected for double piping

// defs used in implementing pipe functionality
#define CHILD 0             // 0 is pid of child process
#define READ 0              // == STDIN_FILENO
#define WRITE 1             // == STDOUT_FILENO

// defs for use in string matching
#define P_STR "|"           // used to string match each input entry
#define QUIT  "quit\n"      // used to string match for closing program

// sig/bool
#define ERR -1              // err code
#define FALSE 0
#define TRUE 1

// tags used when outputting messages to stdout
#define DP_TAG  "doPipe:"
#define HI_TAG  "handleInput:"
#define PO_TAG "pipeOnce:"
#define PT_TAG "pipeTwice:"
#define RC_TAG  "runCmd:"
#define TAG  "myshell: "

// function signatures
char **splitFile(char *input);
char *cleanString(char *str);
int elemCount(char **arr);
int foundFile(char *filePath);
void doPipe(char **cmdArgs, int size);
void handleInput(char *input);
void pipeHandler(char *cmdArgs[], int argCount);
void runCmd(char *initCmd, char *txtPath);
void pipeOnce(char **fArgs, char **sArgs);
void pipeTwice(char **fArgs, char **sArgs, char **tArgs);


int main(int argc, char **argv) {
    char input[MAX_INP_LEN];
    printf("myshell can run 3 commands: my-cat, my-uniq & my-wc. They are designed to have the same base \n"
           "functionality as their unix counterparts. You may also pipe these commands but the shell\n"
           "will not accept more than two pipes.\n");
    int comp = 1;
    while (comp != 0) {
        printf("\nmyprompt> ");
        fgets(input, MAX_INP_LEN, stdin);
        comp = strcmp(QUIT, input);
        if (comp == 0) {
            printf("\nSee ya!\n");
            exit(0);
        } else handleInput(input);
    }
}


void handleInput(char *input) {
    if (strcmp(input, "\n") != 0 && (input != NULL)) { // make sure theres some input
        char *space = strchr(input, ' ');
        if (space == NULL)
            printf("%s%s incorrect arg format or unrecognized command, try again\n", TAG, HI_TAG);

        else {
            char **split = splitFile(input);
            int items = elemCount(split);
            char *initCmd = cleanString(split[0]);
            if (foundFile(initCmd) == ERR) printf("\n%s%s oh no can't find file %s\n", TAG, HI_TAG, initCmd);
            else {
                char *txtPath = cleanString(split[1]);

                // NO_PIPE = 2 args ==> cmd1 & path
                // SINGLE_PIPE = 4 args ==> cmd1, path, pipe & cmd2
                // DOUBLE_PIPE = 6 args ==> cmd1, path, pipe1, cmd2, pipe2 & cmd3
                if (items == NO_PIPE) { // basic command
                    runCmd(initCmd, txtPath);
                } else if (items == SINGLE_PIPE || items == DOUBLE_PIPE) {
                    doPipe(split, items);
                } else printf("%s%s Incorrect number of args supplied\n", TAG, HI_TAG);

            }
        }
    }

}


void doPipe(char **cmdArgs, int size) {
    char *init = cleanString(cmdArgs[0]), *path = cleanString(cmdArgs[1]);
    char *p1 = cleanString(cmdArgs[2]), *secCmd = cleanString(cmdArgs[3]);
    char *fArgs[] = {init, path, NULL}, *sArgs[] = {secCmd, NULL};

    if (strcmp(p1, P_STR) != 0) {
        printf("%s%s expected a pipe but got '%s'\n", TAG, DP_TAG, p1);
        return;
    }
    if (foundFile(secCmd) == ERR) {
        printf("\n%s%s can't find file '%s'\n", TAG, DP_TAG, secCmd);
        return;
    }
    if (size == SINGLE_PIPE) {
        pipeOnce(fArgs, sArgs);
    } else if (size == DOUBLE_PIPE) {
        char *p2 = cleanString(cmdArgs[4]), *thirdCmd = cleanString(cmdArgs[5]);
        char *tArgs[] = {thirdCmd, NULL};

        if (strcmp(p2, P_STR) != 0) {
            printf("%s%s expected a pipe but got '%s'\n", TAG, DP_TAG, p2);
            return;
        }
        if (foundFile(thirdCmd) == ERR) {
            printf("\n%s%s can't find file %s\n", TAG, DP_TAG, thirdCmd);
            return;
        }
        pipeTwice(fArgs, sArgs, tArgs);

    } else {
        printf("%s%s encountered issue, hit last else.\n", TAG, DP_TAG);
    }
}


void pipeOnce(char **fArgs, char **sArgs) {
    // idea: fork & have child run the cmd 1 (writer) -> direct stdout
    // to write end -> close read end -> run cmd 1.
    // next: fork & have child run cmd 2 (reader) -> direct stdin
    // to read end -> close write end -> run cmd 2 which will be
    // able to look for data in stdin
    int desc[2];
    pipe(desc);
    pid_t writer, reader;
    int stat = pipe(desc);
    if (stat == ERR) {
        printf("%s%s error while piping\n", TAG, DP_TAG);
    }
    writer = fork();
    if (writer < CHILD) {
        printf("%s%s problem while making fork for writer process\n", TAG, DP_TAG);
    } else if (writer == CHILD) {
        close(desc[READ]);
        int dStat = dup2(desc[WRITE], WRITE);
        if (dStat == ERR) {
            printf("%s%s dup2 failed on redirecting output\n", TAG, DP_TAG);
        }
        execv(fArgs[0], fArgs);
    } else {
        reader = fork();
        if (reader < CHILD) printf("%s%s problem while making fork for reader process\n", TAG, DP_TAG);
        else if (reader == CHILD) {
            sleep(1);
            close(desc[WRITE]);
            int rStat = dup2(desc[READ], READ);
            if (rStat == ERR) printf("%s%s dup2 failed on redirecting input\n", TAG, DP_TAG);
            execv(sArgs[0], sArgs);
        } else {
            close(desc[WRITE]);
            close(desc[READ]);

            sleep(2);
            kill(writer, SIGUSR1);
            kill(reader, SIGUSR1);
            wait(NULL);
        }
    }

}


void pipeTwice(char **fArgs, char **sArgs, char **tArgs) {
    // idea: fork & have child run the cmd 1  -> direct stdout
    // to write end -> close read end -> run cmd 1.
    // next: fork & have child run cmd 2 -> direct stdin
    // to read end -> direct stdout to write end -> run cmd 2.
    // next: fork & have child run cmd 3 -> direct stdin
    // to read end -> close write end -> run cmd 3
    int d1[2], d2[2];
    pid_t writer, middleMan, reader;

    int stat = pipe(d1);
    if (stat == ERR) {
        printf("%s%s error while piping\n", TAG, DP_TAG);
    }
    writer = fork();
    if (writer < CHILD) {
        printf("%s%s problem while making fork for writer process\n", TAG, DP_TAG);

    }
}


void runCmd(char *initCmd, char *txtPath) {
    __pid_t forked = fork();
    char run[MAX_INP_LEN + 3] = "./";
    strcat(run, initCmd);

    if (forked == 0) {
        char *args[] = {run, txtPath, NULL};
        execvp(args[0], args);
        printf("\n");
    } else if (forked < 0) {
        printf("%s%s encountered error while forking", TAG, RC_TAG);
    } else {
        waitpid(forked, 0, 0);
        kill(forked, SIGUSR1);
    }
}


void pipeHandler(char *cmdArgs[], int argCount) {

}


int foundFile(char *filePath) {
    if (access(filePath, R_OK) != ERR) return TRUE;
    else return ERR;
}


int elemCount(char **arr) {
    int i = 0;
    for (; i < MAX_ARG_LEN; i++) {
        if ((arr[i] == NULL) || strcmp(arr[i], "\n") == 0 || strcmp(arr[i], " ") == 0) break;
    }
    return i;
}


char **splitFile(char *input) {
    char *splitStr = strtok(input, " ");
    char **runInfo = malloc(MAX_ARG_LEN * (sizeof(char *)));
    int cmdCount = 0;
    while (splitStr != NULL) {
        if (cmdCount > MAX_ARG_LEN) break;
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