#include "types.h"
#include "stat.h"
#include "user.h"
#include "userproc.h"
#include "param.h"

int
main(void)
{
	char* state[] = {"UNUSED","EMBRYO","SLEEPING","RUNNABLE","RUNNING","ZOMBIE"};
	struct userproc up;
 	int i;
	for(i=0;i<NPROC;i++)
	{
		if(getprocinfo(i,&up) != -1)
		{
			printf(1,"Process name: %s\n",up.p_name);
			printf(1,"Process id: %d\n",up.p_id);
			printf(1,"Parent process id: %d\n",up.parent_id);
			printf(1,"Size of memory: %d bytes\n",up.size);
			printf(1,"State: %s\n",state[up.state]);
			printf(1,"Killed: %s\n",up.killed != 0 ? "YES" : "NO");
			//if(up.channel != 0
			printf(1,"Waiting on channel: %s\n", up.channel !=0 ? "YES" : "NO");
			printf(1,"\n");
		}
	}
	exit();
}
