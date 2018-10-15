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
main(void)
{
	/* Insert your code here */
	char* argv[3] = {"grep","|",NULL};
	char* argv2[2] = {"ls",NULL};
	int p[2];
	pipe(p);
	int pid = fork();
	if(pid == 0)
	{
		close(p[1]);
		dup2(p[0],0);
		close(p[0]);
		execvp("grep",argv);
		perror("Exec for grep failed\n");
	}
	else
	{
		close(p[0]);
		dup2(p[1],1);
		close(p[1]);
		execvp("ls",argv2);
		perror("Exec for ls failed\n");
	}
	exit(0);
}
