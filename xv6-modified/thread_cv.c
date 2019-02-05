#include "types.h"
#include "stat.h"
#include "user.h"
//#include "thread_spinlock.h"
#include "thread_q.h"
//Spinlock includes from here
//#include "defs.h"
//#include "param.h"
#include "x86.h"
/*#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

*/
//Spinlock includes to here


struct balance {
    char name[32];
    int amount;
};

volatile int total_balance = 0;

struct q q2;
//
int arr[10] = {2,4,6,8,10,12,14,16,18,20};

void printArray()
{
	int i;
	for(i=0;i<10;i++)
	printf(1,"%d ",arr[i]);
	printf(1,"\n");
}

void thread_mutex_init(struct thread_mutex *m)
{
	m->locked = 0;
	m->name = "thread";
}

void thread_mutex_lock(struct thread_mutex *m)
{
	while(xchg(&m->locked, 1) != 0)
	{
		//printf(1,"");
		sleep(1);
	}

	//m->locked = 1;
}

void thread_mutex_unlock(struct thread_mutex *m)
{
	m->locked = 0;
}
volatile unsigned int delay (unsigned int d) {
   unsigned int i; 
   for (i = 0; i < d; i++) {
       __asm volatile( "nop" ::: );
   }

   return i;   
}

void thread_cond_init(struct thread_cv *cv)
{
	cv->channel = 20;
}


void* send(struct q *q1,void* p)
{
	int count = 0;
	int* arr = (int*)p;
	
//	printf(1,"In send in thread_cv.c value of q1->m.name :%s\n",(q1->m).name);
//	printf(1,"In send, thread_create_send works\n");
	while(1)
	{
	//	printf(1,"arr[%d] :%d\n",count,p1[count]);
		thread_mutex_lock(&q1->m);
		while(q1->ptr != 0)
		{
			thread_cond_wait(&q1->cv,&q1->m);
		}
		int *p1 = &arr[count];
		q1->ptr = (void*)p1;
		count++;
		printf(1,"Sent from array : %d\t", *(int*)q1->ptr);
		thread_cond_signal(&q1->cv);
		thread_mutex_unlock(&q1->m);
		if(count == 10)
		thread_exit();
	}
	return 0;
}

void* receive(struct q *q1)
{
//	printf(1,"In receive in thread_cv.c value of q1->m.name :%s\n",(q1->m).name);
	int count1 = 0;
	void* p;
	while(1)
	{
		thread_mutex_lock(&q1->m);
		while((p = q1->ptr) == 0)
		thread_cond_wait(&q1->cv,&q1->m);
		q1->ptr = 0;
		count1++;
		thread_cond_signal(&q1->cv);
		printf(1,"Received from q : %d\n", *(int*)p);
		thread_mutex_unlock(&q1->m);
		if(count1 == 10)
		thread_exit();
	}
	return 0;
}
int main(int argc, char *argv[]) {

 
  void *s1, *s2;
  int t1, t2, r1, r2;

  s1 = malloc(4096);
  s2 = malloc(4096);
  thread_cond_init(&q2.cv);
  thread_mutex_init(&q2.m);
  printf(1,"Using condition variables to send and receive data\n");
  printf(1,"Sending data from array\n");
  printArray();
  t1 = thread_create_sender(send, &q2,(void*)arr, s1);
  t2 = thread_create_receiver(receive, &q2, s2); 
  //printf(1,"t1: %d\nt2: %d\n",t1,t2);
  r1 = thread_join();
  r2 = thread_join();
  
  printf(1, "Threads finished: (%d):%d, (%d):%d\n", 
      t1, r1, t2, r2);
 // printf(1,"total_balance at the end of main : %d\n",total_balance);
  exit();
}
