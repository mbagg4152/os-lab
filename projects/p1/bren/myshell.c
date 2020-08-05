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

#define inputLimit  240
const int argLengthLim = 80;
const int argCountLim = 5;
char *prompt = "\nmyprompt> ";
useconds_t sleepTime = 400;

int chSize = sizeof (char);

void connect_pipes (pid_t pid_conn, char **args, int *fst_ends, int *snd_ends);
void do_pipe (char **cmd_args, int size);

void pipe_once (char **fst_args, char **snd_args);
void pipe_twice (char **fst_args, char **snd_args, char **thd_args);
void read_second_pipe (pid_t pid_read, char **args, int *fst_ends, int *snd_ends);
void read_single_pipe (pid_t pid_read, char **args, int *pipe_ends);
void run_cmd (char *init_cmd, char *txt_path);
void write_pipe (pid_t pid_write, char **args, int *pipe_ends);


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
							pipe_twice (args1, args2, args3);

						}
						else
						{
							char *args1[] = { cmd1, inpPath, NULL };
							char *args2[] = { cmd2, NULL };
							pipe_once (args1, args2);
						}

					}
					else
					{
						run_cmd (cmd1, inpPath);
					}

				}
			}

		}

	}

}


void run_cmd (char *init_cmd, char *txt_path)
{
	char run[inputLimit + 3] = "./";
	strcat (run, init_cmd);
	__pid_t forked = fork ();
	if (forked == 0)
	{
		char *args[] = { run, txt_path, NULL };
		execvp (args[0], args);
		printf ("\n");
	}
	else if (forked < 0)
	{
		printf ("encountered error while forking");
	}
	else
	{
		waitpid (forked, 0, 0);
		kill (forked, SIGUSR1);
	}
}


void pipe_once (char **fst_args, char **snd_args)
{
	int pipe_ends[2];
	pipe (pipe_ends);
	pid_t writer = -1;
	pid_t reader = -1;
	write_pipe (writer, fst_args, pipe_ends);
	reader = fork ();
	if (reader == 0)
	{
		usleep (200);
		close (pipe_ends[1]);
		dup2 (pipe_ends[0], 0);
		execv (snd_args[0], snd_args);
	}
	else
	{
		wait (NULL);
	}
	usleep (200);
	close (pipe_ends[1]);
	close (pipe_ends[0]);
	wait (NULL);

}


void pipe_twice (char **fst_args, char **snd_args, char **thd_args)
{
	int fst_ends[2], snd_ends[2];
	pid_t writer = -1;
	pid_t conn = -1;
	pid_t reader = -1;

	write_pipe (writer, fst_args, fst_ends);
	close (fst_ends[1]);

	pipe (snd_ends);
	conn = fork ();

	if (conn == 0)
	{
		usleep(sleepTime);
		dup2 (fst_ends[0], 0);
		dup2 (snd_ends[1], 1);

		close (fst_ends[1]);
		close (fst_ends[0]);
		close (snd_ends[0]);
		close (snd_ends[1]);
		execv (snd_args[0], snd_args);
	}
	close (fst_ends[0]);
	close (snd_ends[1]);

	reader = fork ();
	if (reader < 0)
	{
		printf ("problem while making fork for reader process\n");
	}
	else if (reader == 0)
	{
		usleep (sleepTime);
		dup2 (snd_ends[0], 0);
		close (fst_ends[0]);
		close (fst_ends[1]);
		close (snd_ends[0]);
		close (snd_ends[1]);
		execv (thd_args[0], thd_args);

	}
	close (snd_ends[0]);

	usleep (sleepTime);
	wait (NULL);
	wait (NULL);
}


void write_pipe (pid_t pid_write, char **args, int *pipe_ends)
{
	usleep (sleepTime);
	pipe (pipe_ends);
	pid_write = fork ();
	if (pid_write == 0)
	{
		close (pipe_ends[0]);
		dup2 (pipe_ends[1], 1);
		execv (args[0], args);
	}
}


void read_single_pipe (pid_t pid_read, char **args, int *pipe_ends)
{
	pid_read = fork ();
	if (pid_read == 0)
	{
		usleep (sleepTime);
		close (pipe_ends[1]);
		dup2 (pipe_ends[0], 0);
		execv (args[0], args);
	}
	else
	{
		wait (NULL);
	}

}


void read_second_pipe (pid_t pid_read, char **args, int *fst_ends, int *snd_ends)
{
	pid_read = fork ();
	if (pid_read < 0)
	{
		printf ("problem while making fork for reader process\n");
	}
	else if (pid_read == 0)
	{
		usleep (sleepTime);

		dup2 (snd_ends[0], 0);

		close (fst_ends[0]);
		close (fst_ends[1]);
		close (snd_ends[0]);
		close (snd_ends[1]);
		execv (args[0], args);

	}
}


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
		}
		else
		{
			printf ("\n\nDone.\n");
			return 0;
		}
	}
}