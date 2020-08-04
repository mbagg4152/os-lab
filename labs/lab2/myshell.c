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
#include <fcntl.h>

#define MAX_LEN 90
#define CMD_LEN 20
#define TRUE 1
#define ERR (-1)
#define FALSE 0
#define KEY  "quit"

int in_dir(char *path);
void run_sort(char *cmd_name, char *txt_path, int write_flag, char *out_path);


int main(int argc, char **argv) {
    char input[MAX_LEN];

    char arg_1[CMD_LEN] = "";
    char arg_2[CMD_LEN] = "";
    char arg_3[CMD_LEN] = "";
    char arg_4[CMD_LEN] = "";
    int comp = 1;
    while (comp != 0) {
        printf("myprompt> ");
        fgets(input, MAX_LEN, stdin);
        sscanf(input, "%s %s %s %s", arg_1, arg_2, arg_3, arg_4);

        comp = strcmp(KEY, arg_1);
        if (comp == 0) {
            printf("\nSee ya!\n");
            exit(0);
        }

        if ((in_dir(arg_1) == ERR) || strlen(arg_1) <= 0) {
            printf("\n%s is unrecognized as a command or program\n", arg_1);
        } else if (in_dir(arg_2) == ERR || strlen(arg_1) <= 0) {
            printf("\ncan't find input file '%s', try again\n", arg_2);
        } else if ((strcmp(arg_3, ">") == 0) && (strlen(arg_4) <= 0)) {
            printf("got empty value after '>'.\n");
        } else if ((strcmp(arg_3, ">") == 0) && strlen(arg_4) > 0) {
            run_sort(arg_1, arg_2, TRUE, arg_4);
        } else {
            printf("\n");
            run_sort(arg_1, arg_2, FALSE, NULL);
            printf("\n");
        }
    }
}


void run_sort(char *cmd_name, char *txt_path, int write_flag, char *out_path) { // create process to run mysort
    pid_t pid = fork();
    char run_arg[CMD_LEN + 3] = "./";
    strcat(run_arg, cmd_name);

    if (pid == 0) { // the child
        char *args[] = {run_arg, txt_path, NULL};
        if (write_flag == TRUE) {
            int fd = open(out_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
            dup2(fd, 1);
            close(fd);
            execvp(args[0], args);
        } else {
            execvp(args[0], args);
        }

    } else waitpid(pid, 0, 0);
}


int in_dir(char *path) {
    if (access(path, F_OK) != ERR) return TRUE;
    else return ERR;
}

