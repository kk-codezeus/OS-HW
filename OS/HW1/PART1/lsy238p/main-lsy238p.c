#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int
main(int argc, char* argv[])
{
	if(argc < 3)
	{
		printf("Less no of args specified exiting ....\n");
		exit(0);
	}
	int pid = fork();
	if(pid < 0)
	{
		printf("Fork not working\n");
		exit(0);
	}
	if(pid == 0) //Child process
	{
		close(1);
		open(argv[2],O_WRONLY | O_CREAT, 0777);
		execlp("ls","ls",NULL);
		printf("Shouldn't come back here otherwise exec not working \n");
	}	
	if(pid > 0)
	{
		pid = wait(NULL);
	}
	

	exit(0);
}
