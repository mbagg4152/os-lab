#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h> /* for strlen */
#include <stdlib.h> /* for exit */
#define HALF_DUPLEX	"/tmp/halfduplex" // path of the pipe

int main(int argc, char *argv[])
{
    int fd;
    char str1[80];

	
	/* Create the first named - pipe */
    int ret_val = mkfifo(HALF_DUPLEX, 0666);

    if ((ret_val == -1) && (errno != EEXIST)) {
        perror("Error creating the named pipe");
        exit(1);
    }

    /* read from the pipe */
	fd = open(HALF_DUPLEX,O_RDONLY); 
	read(fd, str1, 80);	

	// Print the read string and close 
	printf("Client: %s\n", str1); 
	close(fd); 

}