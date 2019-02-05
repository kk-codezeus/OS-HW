#ifndef THREAD_MUTEX_H
#define THREAD_MUTEX_H

struct thread_mutex
{
	uint locked;

	char *name;
};

#endif
