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
#define NO_PIPE 2           // 2 args ==> cmd1 & path
#define SINGLE_PIPE 4       // 4 args ==> cmd1, path, pipe & cmd2
#define DOUBLE_PIPE 6       // 6 args ==> cmd1, path, pipe1, cmd2, pipe2 & cmd3

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
#define DP_TAG  "do_pipe:"
#define IH_TAG  "input_handler:"
#define PO_TAG "pipe_once:"
#define PT_TAG "pipe_twice:"
#define RC_TAG  "run_cmd:"
#define TAG  "myshell: "

// function signatures
char **split_file(char *input);
char *clean_str(char *str);
int elem_count(char **arr);
int found_file(char *filePath);
void do_pipe(char **cmd_args, int size);
void input_handler(char *input);
void connect_pipes(pid_t pid_conn, char **args, int *fst_ends, int *snd_ends);
void pipe_once(char **fst_args, char **snd_args);
void read_single_pipe(pid_t pid_read, char **args, int *pipe_ends);
void read_second_pipe(pid_t pid_read, char **args, int *fst_ends, int *snd_ends);
void pipe_twice(char **fst_args, char **snd_args, char **thd_args);
void write_pipe(pid_t pid_write, char **args, int *pipe_ends);
void run_cmd(char *run, char *txt_path);


int main(int argc, char **argv) {
    char input[MAX_INP_LEN];
    printf("myshell can run 3 commands: my-cat, my-my-uniq & my-wc."
           "\nYou may also pipe these commands but the shell will not"
           " handle more than two pipes.\n");

    int compare_flag = 1; // when this is 0, that means 'quit' has been entered
    while (compare_flag != 0) { // main program/shell loop
        printf("\nmyprompt> ");
        fgets(input, MAX_INP_LEN, stdin);
        compare_flag = strcmp(QUIT, input);
        if (compare_flag == 0) {
            printf("\nSee ya!\n");
            exit(0);
        } else input_handler(input);
    }
}


void input_handler(char *input) {
    if (strcmp(input, "\n") != 0 && (input != NULL)) { // make sure theres some input
        if ((strchr(input, ' ')) == NULL)
            printf("%s%s incorrect arg format or unrecognized command, try again\n", TAG, IH_TAG);

        else {
            char **split_input = split_file(input);
            int arg_cnt = elem_count(split_input);
            char *init_cmd = clean_str(split_input[0]);
            if (found_file(init_cmd) == ERR) printf("\n%s%s oh no can't find file %s\n", TAG, IH_TAG, init_cmd);
            else {
                char run[MAX_INP_LEN + 3] = "./";
                strcat(run, init_cmd);
                char *txt_path = clean_str(split_input[1]);

                if (arg_cnt == NO_PIPE) { // basic command
                    run_cmd(init_cmd, txt_path);
                } else if (arg_cnt == SINGLE_PIPE || arg_cnt == DOUBLE_PIPE) {
                    do_pipe(split_input, arg_cnt);
                } else {
                    printf("%s%s Incorrect number of args supplied. Supplied %d args\n", TAG, IH_TAG, arg_cnt);

                }

            }
        }
    }

}


void run_cmd(char *run, char *txt_path) {
    __pid_t forked = fork();
    if (forked == 0) {
        char *args[] = {run, txt_path, NULL};
        execvp(args[0], args);
        printf("\n");
    } else if (forked < 0) {
        printf("%s%s encountered error while forking", TAG, RC_TAG);
    } else {
        waitpid(forked, 0, 0);
        kill(forked, SIGUSR1);
    }
}


void do_pipe(char **cmd_args, int size) {
    char *fst_cmd = clean_str(cmd_args[0]);
    char *txt_path = clean_str(cmd_args[1]);
    char *pipe_str1 = clean_str(cmd_args[2]);
    char *sec_cmd = clean_str(cmd_args[3]);
    char *fst_args[] = {fst_cmd, txt_path, NULL};
    char *sec_args[] = {sec_cmd, NULL};
    if (strcmp(pipe_str1, P_STR) != 0) {
        printf("%s%s Expected pipe but got '%s'.\n", TAG, DP_TAG, pipe_str1);
        return;
    } else if ((found_file(sec_cmd) == ERR)) {
        printf("%s%s Couldn't find '%s'.\n", TAG, DP_TAG, sec_cmd);
        return;
    } else {
        if (size == SINGLE_PIPE) {
            pipe_once(fst_args, sec_args);
        } else if (size == DOUBLE_PIPE) {
            char *pipe_str2 = clean_str(cmd_args[4]);
            char *thd_cmd = clean_str(cmd_args[5]);
            char *thd_args[] = {thd_cmd, NULL};

            if (strcmp(pipe_str2, P_STR) != 0) {
                printf("%s%s Expected pipe but got '%s'.\n", TAG, DP_TAG, pipe_str2);
                return;
            } else if (found_file(thd_cmd) == ERR) {
                printf("%s%s Couldn't find '%s'.\n", TAG, DP_TAG, sec_cmd);
                return;
            } else pipe_twice(fst_args, sec_args, thd_args);

        } else {
            printf("%s%s encountered issue.\n", TAG, DP_TAG);
            return;
        }
    }

}


void pipe_once(char **fst_args, char **snd_args) {
    int pipe_ends[2];
    pipe(pipe_ends);
    pid_t writer = -1;
    pid_t reader = -1;
    write_pipe(writer, fst_args, pipe_ends);
    read_single_pipe(reader, snd_args, pipe_ends);
    sleep(2);
    close(pipe_ends[WRITE]);
    close(pipe_ends[READ]);
    wait(NULL);

}


void pipe_twice(char **fst_args, char **snd_args, char **thd_args) {
    int fst_pipe_ends[2], snd_pipe_ends[2];
    pid_t writer = -1;
    pid_t reader = -1;
    pid_t conn = -1;

    write_pipe(writer, fst_args, fst_pipe_ends);
    close(fst_pipe_ends[WRITE]);

    connect_pipes(conn, snd_args, fst_pipe_ends, snd_pipe_ends);
    close(fst_pipe_ends[READ]);
    close(snd_pipe_ends[WRITE]);

    read_second_pipe(reader, thd_args, fst_pipe_ends, snd_pipe_ends);
    close(snd_pipe_ends[READ]);

    sleep(2);
    wait(NULL);
    wait(NULL);
}


/* Used to write to pipe for both single and double pipe implementation */
void write_pipe(pid_t pid_write, char **args, int *pipe_ends) {
    if (pipe(pipe_ends) == ERR) {
        printf("%s%s error while piping\n", TAG, PO_TAG);
        return;
    }
    pid_write = fork();
    if (pid_write < CHILD) {
        printf("%s%s problem while making fork for writer process\n", TAG, PO_TAG);
        return;
    } else if (pid_write == CHILD) {
        close(pipe_ends[READ]);
        if (dup2(pipe_ends[WRITE], WRITE) == ERR) {
            printf("%s%s dup2 failed on redirecting output\n", TAG, PO_TAG);
            return;
        }
        execv(args[0], args);
    }
}


/* Used to read from pipe for single pipe implementation only */
void read_single_pipe(pid_t pid_read, char **args, int *pipe_ends) {
    pid_read = fork();
    if (pid_read < CHILD) printf("%s%s problem while making fork for reader process\n", TAG, PO_TAG);
    else if (pid_read == CHILD) {
        sleep(1);
        // no need for write end
        close(pipe_ends[WRITE]);
        // try to redirect stdin to read end of pipe
        if (dup2(pipe_ends[READ], READ) == ERR) {
            printf("%s%s dup2 failed on redirecting input\n", TAG, PO_TAG);
        }
        execv(args[0], args);
    } else {
        wait(NULL);
    }

}


/* Used to read from the first pipe and write to the second pipe */
void connect_pipes(pid_t pid_conn, char **args, int *fst_ends, int *snd_ends) {
    if (pipe(snd_ends) == ERR) {
        printf("%s error while piping\n", TAG);
    }
    pid_conn = fork();
    if (pid_conn < CHILD) {
        printf("%s problem while making fork for writer process\n", TAG);
    } else if (pid_conn == CHILD) {
        sleep(1);
        if ((dup2(fst_ends[READ], READ) == ERR) || (dup2(snd_ends[WRITE], WRITE) == ERR)) {
            printf("%s error with dup2\n", TAG);
            return;
        }
        close(fst_ends[WRITE]);
        close(fst_ends[READ]);
        close(snd_ends[READ]);
        close(snd_ends[WRITE]);
        execv(args[0], args);
    }

}


/* Used to read from second pipe only. The main difference between this function
 * and read_single_pipe is that this function needs to account for more pipes
 * that it needs to close. */
void read_second_pipe(pid_t pid_read, char **args, int *fst_ends, int *snd_ends) {
    pid_read = fork();
    if (pid_read < CHILD) {
        printf("%s problem while making fork for reader process\n", TAG);
    } else if (pid_read == CHILD) {
        sleep(1);
        // try to redirect stdin to read end of pipe
        if (dup2(snd_ends[READ], READ) == ERR) {
            printf("%s dup2 failed on redirecting input\n", TAG);
        }

        // close both pipes
        close(fst_ends[READ]);
        close(fst_ends[WRITE]);
        close(snd_ends[READ]);
        close(snd_ends[WRITE]);
        execv(args[0], args);
    }

}


int found_file(char *filePath) {
    if (access(filePath, R_OK) != ERR) return TRUE;
    else return ERR;
}


int elem_count(char **arr) {
    int i = 0;
    for (; i < MAX_ARG_LEN + 1; i++) {
        if ((arr[i] == NULL) || strcmp(arr[i], "\n") == 0 || strcmp(arr[i], " ") == 0) break;
    }
    return i;
}


char **split_file(char *input) {
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
    //printf("cmdCount: %d\n", cmdCount);
    return runInfo;
}


char *clean_str(char *str) {
    char *stripped = malloc(strlen(str));
    int new = 0, old = 0;
    for (; old < strlen(str); old++, new++) {
        if (!isspace(str[old])) stripped[new] = str[old];
        else new--;
    }
    stripped[new] = 0;
    return stripped;
}