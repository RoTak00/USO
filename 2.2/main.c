#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>


int myeof(int fd)
{

	struct stat* statbuf;
	if((statbuf = (struct stat*)malloc(sizeof(struct stat))) == NULL)
		return -1;
	
	if(fstat(fd, statbuf) == -1)
	{
		free(statbuf);
		return -1;
	}
	
	long int file_size = statbuf->st_size;
	free(statbuf);
	long int offset = lseek(fd, 0, SEEK_CUR);
	if(offset < 0) return -1;
	
	if(file_size == offset)
		return 1;
	return 0;
	
}

int main()
{

	if(isatty(0))
	{
		fprintf(stderr, "Attempted input from terminal.\n");
		exit(EXIT_FAILURE);
	}
	errno = 0;

	char b;
	do
	{
		read(0, &b, 1);
		write(1, &b, 1);
	}
	while(!myeof(0));

	if(errno != 0)
	{
		perror("err");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
	
}