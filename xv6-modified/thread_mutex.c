#include "types.h"
#include "stat.h"
#include "user.h"
//#include "thread_spinlock.h"
#include "thread_mutex.h"
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

//struct thread_spinlock lock;
struct thread_mutex mutex;
//Mutex functions from here

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

void do_work(void *arg){
    //register int* esp asm("esp");
    //printf(1,"here esp: %x\n",esp);
    int i; 
    int old;
    delay(100000);
    struct balance *b = (struct balance*) arg; 
    printf(1, "Starting do_work: s:%s\n", b->name);

                
    for (i = 0; i < b->amount; i++) {
         thread_mutex_lock(&mutex);
         old = total_balance;
         delay(100000);
         total_balance = old + 1;
	 thread_mutex_unlock(&mutex);
   }
  
    
    printf(1, "Done s:%s\n", b->name);
    thread_exit();
    return;
}

int main(int argc, char *argv[]) {

  struct balance b1 = {"b1", 3200};
  struct balance b2 = {"b2", 2800};
 
  void *s1, *s2;
  int t1, t2, r1, r2;

  s1 = malloc(4096);
  s2 = malloc(4096);

  thread_mutex_init(&mutex);
  //printf(1,"Lock name:%s\n",lock.name);

  //printf(1,"fp1: %x\t fp2: %x\n",fp1,fp2); 
  printf(1,"Synchronizing using mutex\n");
  t1 = thread_create(do_work, (void*)&b1, s1);
  t2 = thread_create(do_work, (void*)&b2, s2); 

  //printf(1,"t1: %d\nt2: %d\n",t1,t2);
  r1 = thread_join();
  r2 = thread_join();
  
  printf(1, "Threads finished: (%d):%d, (%d):%d, shared balance:%d\n", 
      t1, r1, t2, r2, total_balance);
 // printf(1,"total_balance at the end of main : %d\n",total_balance);
  exit();
}
