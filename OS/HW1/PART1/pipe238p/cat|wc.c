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
	int fd[2];
	pipe(fd);
	int pid = fork();
	if(pid < 0)
	{
		perror("Fork error... exiting\n");
		exit(0);
	}
	if(pid == 0)
	{
		dup2(fd[1],1);
		close(fd[0]);
		close(fd[1]);
		execvp("ls",argv);
		exit(0);
	}
	else
	{
		close(fd[1]);
	}

/*	switch(pid)
    {
        case -1:
            perror("Forkscrew");
            exit(1);
            //break; //unreachable//
        case 0:
            dup2(fd[1], 1); //NOW we dup2()//
            close(fd[0]); //no need to pass these file descriptors to...//
            close(fd[1]); //...a program that doesn't expect to have them open// 
            execvp("ls", argv);
            exit(0); //might want an error message//
            //break; //unreachable//
       default:
            //waitpid(-1, &status, 0); //don't wait yet//
            //dup2(old_std_out, 1);
            close(fd[1]); //we don't need this in the parent anymore//
            break;
    }*/

	pid = fork();
	if(pid < 0)
	{
		perror("Fork2 error exiting....\n");
		exit(0);
	}
	if(pid == 0)
	{
		dup2(fd[0],0);
		close(fd[0]);
		execvp("cat",argv);
		exit(0);
	}


	/*switch(pid)
    {
        case -1:
            perror("Forkscrew");
            //might want to ensure the first child can terminate//
            exit(1);
            //break; //unreachable//
        case 0:
            dup2(fd[0], 0);
            close(fd[0]); //again, cat doesn't expect a fourth fd open//
            execvp("cat", argv);
            //again, error message would be nice//
            exit(0);
            //break;
        default:
            //waitpid(-1, &status, 0);
            //dup2(old_std_in, 0);
            break;
    }*/
	wait(NULL);
	wait(NULL);
	printf("Hallelujah\n");
	exit(0);
}
