/*
	Brendan Gallagher
   	CS-451 project 1
*/


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>

#define inputLimit  500 // size of user input
const int argLengthLim = 100; // size of each arg
int chSize = sizeof (char);
char *prompt = "\nmyprompt> "; // prompt message
useconds_t sleepTime = 400; // time to use with usleep

/*
	singleCommand: takes path of command and input file,
 	then forks and runs the program as child process.
 */
void singleCommand (char *command, char *filePath)
{
	pid_t pidt = fork ();
	if (fork () == 0)
	{
		char usable[inputLimit + 3] = "./";
		strcat (usable, command);
		char *args[] = { usable, filePath, NULL };
		execvp (usable, args);
	}
	else
	{
		waitpid (pidt, 0, 0);
		kill (pidt, SIGUSR1);
	}
}


/*
 	writeToPipe: used to redirect output of the first command
 	for both piping functions.
 */
void writeToPipe (char **args1, int *fd)
{
	usleep (sleepTime);
	pipe (fd); // make pipe
	if (fork () == 0)
	{
		/* redirect stdout, close read end and run */
		dup2 (fd[1], 1);
		close (fd[0]);
		execv (args1[0], args1);
	}
	else
	{
		printf ("something went wrong with pipe.\n");
	}
}


/*
 	onePipe: executes and handles execution of a pipe
 	and two commands.
 */
void onePipe (char **args1, char **args2)
{
	int fd[2]; // file descriptors
	pipe (fd); // make pipe

	writeToPipe (args1, fd); // run first and write to pipe
	if (fork () == 0)
	{
		usleep (200);

		/* redirect stdin, close write end and run cmd */
		dup2 (fd[0], 0);
		close (fd[1]);
		execv (args2[0], args2);
	}
	else // parent
	{
		wait (NULL);
	}

	usleep (sleepTime);

	/* close unused pipes */
	close (fd[0]);
	close (fd[1]);
	wait (NULL);

}


/*
 	twoPipes: executes and handles execution of 2 pipes
 	and three commands.
 */
void twoPipes (char **args1, char **args2, char **args3)
{
	int fd1[2], fd2[2]; // descriptors

	writeToPipe (args1, fd1);
	close (fd1[1]);

	pipe (fd2); // make second pipe

	if (fork () == 0)
	{
		usleep (sleepTime);
		/* redirect read end of first and write end of second*/
		dup2 (fd1[0], 0);
		dup2 (fd2[1], 1);

		/* make sure to close all pipes */
		close (fd1[1]);
		close (fd1[0]);
		close (fd2[0]);
		close (fd2[1]);
		execv (args2[0], args2);
	}
	close (fd1[0]);
	close (fd2[1]);

	if (fork () == 0)
	{
		usleep (sleepTime);

		/* redirect for second pipe only and then close both pipes */
		dup2 (fd2[0], 0);
		close (fd1[1]);
		close (fd1[0]);
		close (fd2[1]);
		close (fd2[0]);

		execv (args3[0], args3);

	}
	close (fd2[0]);

	usleep (sleepTime);
	wait (NULL);
	wait (NULL);
}


/*
 	determineRunType: given the user input, decide if valid. if valid, determine
 	if command is a single command, one pipe or two pipes.
 */
void determineRunType (char *cmd1, char *inpPath, char *pipe1, char *cmd2, char *pipe2, char *cmd3)
{
	if (strlen (cmd1) < 1)
	{
		fprintf (stderr, "bad input.\n");
		return;
	}
	else
	{
		if (access (cmd1, R_OK) == -1)
		{
			fprintf (stderr, "command 1 not recognized.\n");
			return;
		}
		else
		{
			if (strlen (inpPath) < 1)
			{
				fprintf (stderr, "path for input file must be supplied.\n");
				return;
			}
			else
			{
				if (access (inpPath, R_OK) == -1)
				{
					fprintf (stderr, "cannot access input file\n");
					return;
				}
				else
				{
					if (strchr (pipe1, '|') != NULL)
					{

						if (strlen (cmd2) < 1 || isblank(cmd2[0]) || !isalnum(cmd2[0]))
						{
							fprintf (stderr, "expected value after first pipe.\n");
							return;
						}
						if (access (cmd2, R_OK) == -1)
						{
							fprintf (stderr, "command 2 not recognized.\n");
							return;
						}

						if (strchr (pipe2, '|') != NULL)
						{
							if ((strlen (cmd3) < 1) || isblank(cmd3[0]) || !isalnum(cmd3[0]))
							{
								fprintf (stderr, "expected value after second pipe.\n");
								return;
							}
							if (access (cmd3, R_OK) == -1)
							{
								fprintf (stderr, "command 3 not recognized.\n");
								return;
							}

							char *args1[] = { cmd1, inpPath, NULL };
							char *args2[] = { cmd2, NULL };
							char *args3[] = { cmd3, NULL };
							twoPipes (args1, args2, args3);

						}
						else
						{
							char *args1[] = { cmd1, inpPath, NULL };
							char *args2[] = { cmd2, NULL };
							onePipe (args1, args2);
						}

					}
					else
					{
						singleCommand (cmd1, inpPath);
					}

				}
			}

		}

	}

}


/*
 	main: contains the main loop of the shell and is in charge of getting
 	user input.
 */
int main (int argc, char **argv)
{
	printf ("Usage for my-cat, my-wc and my-uniq\n"
			"\t1. command 1 <text-file-name>\n"
			"\t2. command 1 <text-file-name> | command 2\n"
			"\t3. command 1 <text-file-name> | command 2 | command 3\n");

	char str[inputLimit];
	char cmd1[argLengthLim];
	char cmd2[argLengthLim];
	char cmd3[argLengthLim];
	char inpPath[argLengthLim];
	char pipe1[argLengthLim];
	char pipe2[argLengthLim];


	int compare_flag = 1; // when this is 0, that means 'quit' has been entered
	while (1)
	{
		printf ("%s", prompt);
		fgets (str, inputLimit, stdin);
		sscanf (str, "%s %s %s %s %s %s", cmd1, inpPath, pipe1, cmd2, pipe2, cmd3);

		if (strcmp ("quit", cmd1) != 0)
		{
			determineRunType (cmd1, inpPath, pipe1, cmd2, pipe2, cmd3);
			printf ("\n");
		}
		else
		{
			printf ("\n\nDone.\n");
			return 0;
		}
	}
}