#ifndef USERPROC_H
#define USERPROC_H


struct userproc{			
	char p_name[16];	// Process name
	int p_id; 		// Process id
	int parent_id;		// Parent process id
	uint size;		// Process memory size
	int state; 		// Process state
	void *channel;		//Whether waiting on a channel
	int killed;		//Killed if not equal to 0
};

#endif
