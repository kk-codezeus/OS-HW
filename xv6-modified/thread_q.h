#ifndef THREAD_Q_H
#define THREAD_Q_H
#include "thread_mutex.h"
#include "thread_cv.h"

struct q{
	struct thread_cv cv;
	struct thread_mutex m;
	void* ptr;
};

#endif
