/*
	Brendan Gallagher
   	CS-451 project 1
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

#define charLimit  2000
#define lineLimit 500

char lines[lineLimit][charLimit]; // accessing globally is less trouble than passing this as a value


/*
	filterDupes(int): takes in the number of lines as a param. for each line, it is checked against
 	every other line. if a duplicate is found, then the global lines is updated as well as the
 	removed counter. once done, it returns the removed counter.
 */
int filterDupes (int length)
{
	int removed = 0;
	/* for each line of input, compare it against the next */
	for (int curr = 0; curr < length; curr++)
	{
		for (int next = curr + 1; next < length;)
		{
			char *this = lines[curr];
			char *nextLine = lines[next];
			if (strcmp (this, nextLine) == 0)
			{
				/* if duplicate is found then delete the second duplicate line */
				for (int i = next; i < length; i++)
				{
					char *tmp = lines[i];
					char *nextTmp = lines[i + 1];
					strcpy (tmp, nextTmp);
				}
				removed++;
			}
			else
			{
				next++; // look at next line
			}
		}
	}
	return removed;
}


/*
	processInput(char*): takes in path name. if string is NULL, then check for data
 	in stdin. if no file or data is found program exits. on success, the lines variable
 	is filled with the lines from the file and the number of lines are returned.
 */
int processInput (char *fileName)
{
	FILE *input;
	if (fileName != NULL) // got filename in args
	{
		input = fopen (fileName, "r");
		if (input == NULL)
		{
			fprintf (stderr, "my-uniq: could not find input file.\n");
			exit (1);
		}
	}

	else // check for data in stdin
	{
		input = stdin;
	}

	int length = 0; // counter variable for length of input string array
	while (fgets (lines[length], charLimit, input))
	{
		length++;
	}

	/* replace newlines stripped by fgets */
	for (int i = 0; i < length; i++)
	{
		int lastIndex = (int) strlen (lines[length]) - 1;
		lines[length][lastIndex] = '\0';
		strcat (lines[length], "\n");
	}

	return length;
}


/*
	myUniq(char*): implementation of the unix command uniq. on successfully reading a file,
	program displays unique lines.
*/
void myUniq (char *fileName)
{
	int length = processInput (fileName);
	int removed = filterDupes (length);
	for (int i = 0; i < (length - removed); i++)
	{
		char *ch = lines[i];
		printf ("%s", ch);
	}

}


/*
 	main(int, char**): checks the cmdline for a text file path to read. if no path
 	can be found then it passes NULL to myUniq, which handles this and looks for
 	data in stdin.
*/
int main (int argc, char **argv)
{
	if (argc > 1) // name given
	{
		myUniq (argv[1]);
	}
	else // assume data from pipe
	{
		myUniq (NULL);
	}
	return 0;
}


