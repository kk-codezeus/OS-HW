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
	/* Insert your code here */
	int p[2];
	pipe(p);
	int pid = fork();
	if(pid == 0)
	{
		close(0);
		dup(p[0]);
		close(p[0]);
		close(p[1]);
		execvp("wc",argv);
	}
	else
	{
		close(1);
		dup(p[1]);
		close(p[0]);
		close(p[1]);
		execlp("cat",argv[1],NULL);
	}
	exit(0);
}
