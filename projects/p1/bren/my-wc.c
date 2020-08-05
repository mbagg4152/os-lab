/*
	Brendan Gallagher
   	CS-451 project 1
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>
#include <unistd.h>

char *input; // to hold data from file
int arrSize = 300; // starting value of array size
int charSize = sizeof (char);

/* whitespaces */
char back = '\b';
char end = '\0';
char newLine = '\n';
char ret = '\r';
char space = ' ';
char tab = '\t';


/*
 	fillArr: parses input and then fills up the input array.
 	returns the fileSize of the input.
 */
long fillArr (char *inputPath)
{
	long fileSize;
	FILE *inputFile;
	if (inputPath != NULL)
	{
		{
			/* open input file, go to end and get file size, rewind to be read again */
			inputFile = fopen (inputPath, "r");
			fseek (inputFile, 0, SEEK_END);
			fileSize = ftell (inputFile);
			rewind (inputFile);

			/* allocate space to array and read file data into it*/
			input = malloc (fileSize);
			fread (input, 1, fileSize, inputFile);

			/* close input & return file size */
			fclose (inputFile);
			return fileSize;
		}
	}

	else
	{
		input = (char *) malloc (charSize * arrSize);
		int filled = 0; // used for realloc
		int ch = fgetc (stdin);
		while (!feof (stdin) && !ferror (stdin))
		{
			if (filled == arrSize)
			{
				arrSize *= 2;
				input = realloc (input, (charSize * arrSize));
			}
			input[filled] = (char) ch;
			filled++;
			ch = fgetc (stdin);
		}
		fileSize = filled;
		if (fileSize == 0)
		{
			fprintf (stderr, "my-wc: no file name and stdin is empty.\n");
			exit (1);
		}

	}
	return fileSize;

}


/*
	myWc(char*): takes in the path of a file in order to count number of
 	words and lines. if the path == NULL then look for data in stdin. if
 	no data is found or the file with the supplied name isn't accessible
 	then exit
 */
void myWc (char *path)
{
	int index; // counter
	int wordCount = 0;
	int lineCount = 0;
	long inputLength = fillArr (path);

	/* go through file data and determine when to increase
	 * the word and line counter */
	for (index = 0; index < inputLength; index++)
	{
		char c = input[index];
		if (isspace(c))
		{
			if (index < inputLength - 1)
			{
				char next = input[index + 1];
				if ((c == newLine || c == ret) && (!isalnum(next)))
				{
					lineCount++;
				}

				else // next is alphanum
				{
					/* if next is space or tab then there has been a new word. if next is a newline
 					 * then there has been a new word and line. otherwise, have not reached end of word */
					if (c == tab || c == space)
					{
						wordCount++;
					}
					else if (c == newLine)
					{
						wordCount++;
						lineCount++;
					}
				}
			}
		}

	}

	/* assume starting with alphanum ==> word */
	if (isalnum(input[0]))
	{
		wordCount++;
	}

	printf ("\n%d (lines) %d (words)\n", lineCount, wordCount);

}


/*
	 main(int, char**): checks for a filename supplied in args. if no filename
	 is found, pass NULL to the wc function so it knows to look for data in stdin.
 */
int main (int argc, char **argv)
{
	if (argc > 1) // data from file
	{
		myWc (argv[1]);
	}
	else // data from stdin
	{
		myWc (NULL);
	}
	return 0;
}
