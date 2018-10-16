#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define pipe1_r 0
#define pipe1_w 1
#define pipe2_r 2
#define pipe2_w 3

int
main(void)
{
	/* Insert your code here */
	//Hard coding argument vectors for each process
	char* argv_ls[2] = {"ls",NULL};
	char* argv_grep[3] = {"grep","|",NULL};
	char* argv_wc[2] = {"wc",NULL};

	int pipe_index[4];
	//Defining pipe 1 and error checking 
	if(pipe(pipe_index) < 0)
	{
		perror("First pipe not created exiting...exiting\n");
		exit(0);
	}
	//Defining pipe 2 and error checking
	if(pipe(pipe_index + 2) < 0)
	{
		perror("Second pipe not created.... exiting\n");
		exit(0);
	}
	int pid_ls, pid_grep, pid_wc;
	if((pid_ls = fork()) < 0)	// Foring for ls command(1st command) and error checking fork()
	{
		perror("Fork for ls not working......exiting \n");
		exit(0);
	}
	if(pid_ls == 0)			// ls child process
	{
		// closing unwanted file descriptors for the child process ls	
		close(pipe_index[pipe1_r]);
		close(pipe_index[pipe2_r]);
		close(pipe_index[pipe2_w]);

		if(dup2(pipe_index[pipe1_w],1) < 0) //dup2 fail-safe
		{
			perror("dup2 inside ls not working.... exiting\n");
			exit(0);
		}
		// closing write descriptor of the pipe1 inside ls child
		close(pipe_index[pipe1_w]);
		execvp(argv_ls[0],argv_ls);
		perror("execvp inside ls failed.... exiting\n");		// Error incase exec doesn't work
		exit(0);
	}
	else
	{
		close(pipe_index[pipe1_w]);	// closing the write descriptor for pipe1 for the parent process and the future
	}

	if((pid_grep = fork()) < 0)//Forking for grep process and error checking fork
	{
		perror("Fork for grep process failed..... exiting\n");
		exit(0);
	}
	if(pid_grep == 0)			// Grep child process.
	{
		//close(pipe_index[pipe1_w]);  // already closed the 1st pipe's write descriptor in the parent before calling fork...
		close(pipe_index[pipe2_r]);	// not needed here closing it
		// Redirecting stdin of "grep" process to pipe1 read and error checking dup2
		if(dup2(pipe_index[pipe1_r],0) < 0)
		{
			perror("dup2 for grep process to pipe1 read failed...exiting\n");
			exit(0);
		}	
	        // Redirecting stdout of "grep" process to pipe2 write and error checking dup2
		if(dup2(pipe_index[pipe2_w],1) < 0)
		{
			perror("dup2 for grep process to pipe2 write failed... exiting\n");
			exit(0);
		}
		//Closing both pipes read and write
		close(pipe_index[pipe1_r]);
		close(pipe_index[pipe2_w]);

		execvp(argv_grep[0],argv_grep);
		perror("grep execv failed.. shouldn't come back here.....exiting\n");
		exit(0);
	}
	else
	{	// closing the pipe1 read and pipe2 write in parent process for the future
		close(pipe_index[pipe1_r]);
		close(pipe_index[pipe2_w]);
	}
	if((pid_wc = fork()) < 0) 		// forking check for wc process
	{
		perror("Fork for wc not working....exiting\n");
		exit(0);
	}
	if(pid_wc == 0)
	{
			
			//Redirecting stdin of "wc" process to pipe2_read and error checking dup2
			if(dup2(pipe_index[pipe2_r],0) < 0)
			{
				perror("dup2 for wc process to pipe2 read failed...exiting\n");
				exit(0);
			}	

			//Closing both pipes read and write.
			close(pipe_index[pipe2_r]);

			execvp(argv_wc[0],argv_wc);
			perror("Wc failed...exiting\n");
			exit(0);

	}
	wait(NULL);
	wait(NULL);
	wait(NULL);	
	exit(0);
}
