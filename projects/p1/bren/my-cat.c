/*
	Brendan Gallagher
   	CS-451 project 1
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


/*
 	Replicates the functionality (without flags) of the linux command cat.
 	Reads a file & displays its contents in the terminal.
 */
void catFile (char *fileName)
{
	if (fileName != NULL)
	{
		FILE *inputFile = fopen (fileName, "r");
		if (inputFile == NULL)
		{
			printf ("my-cat: cannot open input file.\n");
			exit (1);
		}

		char pc;
		while (!feof (inputFile))
		{
			fscanf (inputFile, "%c", &pc);
			fputc (pc, stdout);
		}
	}
	else
	{
		int gc = fgetc (stdin);
		if (stdin != NULL)
		{
			while (gc != EOF)
			{
				putchar (gc);
				gc = fgetc (stdin);
			}
		}
		else
		{
			printf ("my-cat: no file path or stdin data has been provided.\n");
			exit (1);
		}
	}
}


/*
 	main: checks for a file path in args. if no path is supplied then pass
 	NULL to myCat which will look for data in stdin.
*/
int main (int argc, char **argv)
{
	char *fileName;
	if (argc == 1)
	{
		fileName = NULL;
	}
	else
	{
		fileName = argv[1];
	}
	catFile (fileName);
}


