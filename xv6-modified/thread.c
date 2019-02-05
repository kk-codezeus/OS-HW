#include "types.h"
#include "stat.h"
#include "user.h"
#include "thread_spinlock.h"
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

struct thread_spinlock lock;
struct thread_mutex mutex;
//mycpu function starts here
/*struct cpu*
mycpu(void)
{
  int apicid, i;
  
  if(readeflags()&FL_IF)
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}


*/
//mycpu function ends here
//Spinlock functions from here
/*
void
pushcli(void)
{
  int eflags;

  eflags = readeflags();
  cli();
  if(mycpu()->ncli == 0)
    mycpu()->intena = eflags & FL_IF;
  mycpu()->ncli += 1;
}

void
popcli(void)
{
  if(readeflags()&FL_IF)
    panic("popcli - interruptible");
  if(--mycpu()->ncli < 0)
    panic("popcli");
  if(mycpu()->ncli == 0 && mycpu()->intena)
    sti();
}


void
getcallerpcs(void *v, uint pcs[])
{
  uint *ebp;
  int i;

  ebp = (uint*)v - 2;
  for(i = 0; i < 10; i++){
    if(ebp == 0 || ebp < (uint*)KERNBASE || ebp == (uint*)0xffffffff)
      break;
    pcs[i] = ebp[1];     // saved %eip
    ebp = (uint*)ebp[0]; // saved %ebp
  }
  for(; i < 10; i++)
    pcs[i] = 0;
}

// Check whether this cpu is holding the lock.
int
holding(struct thread_spinlock *lock)
{
  int r;
  pushcli();
  r = lock->locked && lock->cpu == mycpu();
  popcli();
  return r;
}

*/

void
thread_spin_init1(struct thread_spinlock *lk)
{
  lk->name = "thread";
  lk->locked = 0;
  lk->cpu = 0;
}

// Acquire the lock.
// Loops (spins) until the lock is acquired.
// Holding a lock for a long time may cause
// other CPUs to waste time spinning to acquire it.
void
thread_spin_lock1(struct thread_spinlock *lk)
{
 // pushcli(); // disable interrupts to avoid deadlock.
 // if(holding(lk))
   // panic("acquire");

  // The xchg is atomic.
  while(xchg(&lk->locked, 1) != 0)
    ;

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that the critical section's memory
  // references happen after the lock is acquired.
  __sync_synchronize();

  // Record info about lock acquisition for debugging.
  //lk->cpu = mycpu();
  //getcallerpcs(&lk, lk->pcs);
}

// Release the lock.
void
thread_spin_unlock1(struct thread_spinlock *lk)
{
  //if(!holding(lk))
    //panic("release");

  //lk->pcs[0] = 0;
  //lk->cpu = 0;

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that all the stores in the critical
  // section are visible to other cores before the lock is released.
  // Both the C compiler and the hardware may re-order loads and
  // stores; __sync_synchronize() tells them both not to.
  __sync_synchronize();
  asm volatile("movl $0, %0" : "+m" (lk->locked) : );

  //popcli();
}

//Spinlock functions to here
//
//Mutex functions from here
/*
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
*/
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
	 thread_spin_lock1(&lock);
         old = total_balance;
         delay(100000);
         total_balance = old + 1;
	 thread_spin_unlock1(&lock);
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

  thread_spin_init1(&lock);
  //printf(1,"Lock name:%s\n",lock.name);

  //printf(1,"fp1: %x\t fp2: %x\n",fp1,fp2); 
  printf(1,"Creating two threads and synchronising using spinlocks\n");
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
