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
	int pid_grep, pid_wc;
	if((pid_grep = fork()) < 0)//Forking for grep process and error checking
	{
		perror("Fork for grep process failed..... exiting\n");
		exit(0);
	}
	if(pid_grep == 0)			// Grep child process.
	{
		close(pipe_index[pipe1_w]);
		close(pipe_index[pipe2_r]);
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
		perror("grep execv failed.. shouldn't come back here\n...");
	}
	else
	{
		pid_wc = fork(); //Forking for wc process and error checking
		if(pid_wc < 0)
		{
			perror("Fork for wc process failed.....exiting\n");
			exit(0);
		}
		if(pid_wc == 0)		// WC child  process
		{
			close(pipe_index[pipe1_r]);
			close(pipe_index[pipe1_w]);
			close(pipe_index[pipe2_w]);
			
			//Redirecting stdin of "wc" process to pipe2_read and error checking dup2
			if(dup2(pipe_index[pipe2_r],0) < 0)
			{
				perror("dup2 for wc process to pipe2 read failed...exiting\n");
				exit(0);
			}	

			//Closing both pipes read and write.
			close(pipe_index[pipe2_r]);

			execvp(argv_wc[0],argv_wc);
			perror("Wc failed...\n");

		}
		else 			//Parent process "ls"
		{
			close(pipe_index[pipe1_r]);
			close(pipe_index[pipe2_r]);
			close(pipe_index[pipe2_w]);
			//Redirecting stdout of "ls" process to pipe1_write and error checking dup2
			
			if(dup2(pipe_index[pipe1_w],1) < 0)
			{
				perror("dup2 for ls process to pipe1 write failed...exiting\n");
				exit(0);
			}	

			//Closing both pipes read and write
			close(pipe_index[pipe1_w]);

			execvp(argv_ls[0],argv_ls);
			perror("ls failed...\n");
		}
	}
	
			close(pipe_index[pipe1_r]);
			close(pipe_index[pipe1_w]);
			close(pipe_index[pipe2_r]);
			close(pipe_index[pipe2_w]);
	exit(0);
}
