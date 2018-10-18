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
	/*if(argc < 3)					// if no of args is less than 3, then send error
	{
		printf("Less no of args specified exiting ....\n");
		exit(0);
	}*/
	char * argv_ls[4] = {"ls",">","y",0};
	int pid = fork();
	if(pid < 0)				// Fork error check
	{
		printf("Fork not working\n");
		exit(0);
	}
	if(pid == 0) //Child process
	{
		close(1);					// close the stdout
		open(argv_ls[2],O_WRONLY | O_CREAT, 0777);		// open the lowest open fd(in this case 1) and point it to 
		execlp(argv_ls[0],"ls",NULL);				// execute the ls command
		printf("Shouldn't come back here otherwise exec not working \n");	// in case exec() fails
	}	
	if(pid > 0)
	{
		pid = wait(NULL);					// wait for the child process to finish
	}
	

	exit(0);
}
