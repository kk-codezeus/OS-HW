#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX 512

int
main(int argc, char* argv[])
{
	/* Insert your code here */
	char buff[MAX];
	int fd_r,fd_w;
	if(argc < 3)
	{
		perror("No of args too low.... exiting\n");
		exit(0);
	}
	if((fd_r = open(argv[1], O_RDONLY)) < 0)
	{
		perror("Error opening file to read.... exiting\n");
		exit(0);
	}
	if((fd_w = open(argv[2], O_WRONLY | O_CREAT, 0777)) < 0)
	{
		perror("Error opening file to write.... exiting\n");
		exit(0);
	}
	//printf("fd_r = %d \t fd_w = %d\n",fd_r,fd_w);
	
	int r_count = -1;
	for(;;)
	{
		r_count = read(fd_r,buff,MAX);
		if(r_count < 0)
		{
			perror("Error reading from input file...exiting\n");
			exit(0);
		}
		if(r_count == 0)
		{
			break;
		}
		if(write(fd_w,buff,r_count) != r_count)
		{
			perror("Write error...exiting\n");
			exit(0);
		}
	}
	close(fd_r);	
	close(fd_w);
	exit(0);
}
