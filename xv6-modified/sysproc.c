#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
//Change 
//To access user proc struct defined in the below file
#include "userproc.h"
#include "thread_spinlock.h"
#include "thread_q.h"
#include "balance.h"

// Defining this function in proc.c to access the ptable struct
extern int getprocinfo(int,struct userproc*);
extern int thread_create(void(*fp)(void*),void* arg,void* stack);
extern int thread_join(void);
extern int thread_exit(void);
extern int thread_spin_init(struct thread_spinlock*);
extern int thread_spin_lock(struct thread_spinlock*);
extern int thread_spin_unlock(struct thread_spinlock*);
extern int thread_create_sender(void*(fp)(struct q*,void*),struct q*,void*,void*);
extern int thread_create_receiver(void* (fp)(struct q*,void*),struct q*,void*);
extern void thread_cond_wait(struct thread_cv*,struct thread_mutex*);
//extern void thread_cond_signal(struct thread_cv*);


int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
//Helper function for backtrace to print user registers.

void bt_printreg(struct proc* curproc)
{
	// Registers pushed by pushal
	
	cprintf("eax: 0x%x\n",curproc->tf->eax);
	cprintf("ebx: 0x%x\n",curproc->tf->ebx);
	cprintf("ecx: 0x%x\n",curproc->tf->ecx);
	cprintf("edx: 0x%x\n",curproc->tf->edx);
	cprintf("esi: 0x%x\n",curproc->tf->esi);
	cprintf("edi: 0x%x\n",curproc->tf->edi);
	cprintf("ebp: 0x%x\n",curproc->tf->ebp);

	// Rest of the trapframe
	
	cprintf("ds: 0x%x\n",curproc->tf->ds);
	cprintf("es: 0x%x\n",curproc->tf->es);
	cprintf("fs: 0x%x\n",curproc->tf->fs);
	cprintf("gs: 0x%x\n",curproc->tf->gs);

	// User registers pushed by hardware
	
	cprintf("eip: 0x%x\n",curproc->tf->eip);
	cprintf("cs: 0x%x\n",curproc->tf->cs);
	cprintf("eflags: 0x%x\n",curproc->tf->eflags);
	cprintf("esp: 0x%x\n",curproc->tf->esp);
	cprintf("ss: 0x%x\n",curproc->tf->ss);
}
//Helper function for backtrace to walk the stack frames and print return add.

void bt_printframes(uint ebp)
{
	//cprintf("return address of backtrace: 0x%x\n",*(uint*)(ebp-12));
	//char* base[3] = {"bar","foo","main"};
	//char* ret[3] = {"baz","bar","foo"};
	int i=0;
	//for(i=0;i<3;i++)
	//{
	//	cprintf("base address of %s : 0x%x\n",base[i],*(uint*)ebp);
	//	cprintf("return address of %s: 0x%x\n",ret[i],*(uint*)(ebp+4));
	//	ebp = *(uint*)ebp;
	//}
	cprintf("User stack\n");
	while(*(uint*)(ebp+4) != 0xffffffff)
	{
		cprintf("#%d : 0x%x\n",i,*(uint*)(ebp+4));
		ebp = *(uint*)ebp;
		i++;
	}
	//Return address of main function:
	cprintf("#%d : 0x%x\n",i,*(uint*)(ebp+4));
	cprintf("Kernel stack\n");
	i=0;
	register int *kernel_ebp asm("ebp");
	uint k_ebp = *kernel_ebp;
	while(k_ebp > 0x80000000)
	{
		cprintf("#%d : 0x%x\n",i,*(uint*)(k_ebp+4));
                k_ebp = *(uint*)k_ebp;
                i++;
	}
}

//Backtrace definition which uses two helper functions
int
sys_backtrace(void)
{
	struct proc *curproc = myproc();
	bt_printreg(curproc);
	bt_printframes(curproc->tf->ebp);
	return 0;		
}
// Used to get the details of all the processes.
int sys_getprocinfo(void)
{
	//cprintf("getprocinfo system call setup done\n");
	int process_num;
	struct userproc *uproc;
	//cprintf("%d\n",sizeof(*uproc));

	//Get the value for process_num from user stack 
	if(argint(0, &process_num)<0 || argptr(1,(void*)&uproc,sizeof(*uproc))<0)
		return -1;
	//cprintf("%d\n",process_num);
	return getprocinfo(process_num,uproc);
	 
}

int sys_thread_create(void)
{
	void(*fp)(void*);
	//cprintf("sizeof(fp):%d and sizeof(*fp) %d\n",sizeof(fp),sizeof(*fp));
	void *arg;
	void *stack;
	if(argptr(0,(char**)&fp,4) < 0 || argptr(1,(char**)&arg,36) < 0 || argptr(2,(char**)&stack,4096) < 0)
	      return -1;
	return thread_create(fp,arg,stack);	
}

int sys_thread_join(void)
{
	return thread_join();
}

int sys_thread_exit(void)
{
	//cprintf("sys_thread_exit\n");
	return thread_exit();
}

int sys_thread_create_sender(void)
{
	//cprintf("In sysproc.c sys_thread_create_sender\n");
	void*(*fp)(struct q*,void*);
	struct q* q1 ;
	void *arg;
	void *stack;
	if(argptr(0,(char**)&fp,4) < 0 || argptr(1,(char**)&q1,sizeof(*q1)) < 0|| argptr(2,(char**)&arg,20) < 0 || argptr(3,(char**)&stack,4096) < 0)
	return -1;
	//cprintf("Channel: %d\n Mutex: %s\n",(q1->cv).channel,(q1->m).name);
	//cprintf();
	//int* p1 = (int*) arg;
	//cprintf("Array[5] :%d\n",p1[5]);
	return thread_create_sender(fp,q1,arg,stack);
}

int sys_thread_create_receiver(void)
{
//	cprintf("In sysproc.c sys_thread_create_receiver\n");
//	return 1;
	void*(*fp)(struct q*,void*);
	struct q* q1 ;
	void *stack;
	if(argptr(0,(char**)&fp,4) < 0 || argptr(1,(char**)&q1,sizeof(*q1)) < 0|| argptr(2,(char**)&stack,4096) < 0)
	return -1;
	//cprintf("Channel: %d\n Mutex: %s\n",(q1->cv).channel,(q1->m).name);
	//cprintf();
	//int* p1 = (int*) arg;
	//cprintf("Array[5] :%d\n",p1[5]);
	return thread_create_receiver(fp,q1,stack);
}

int sys_thread_cond_wait(void)
{
	struct thread_mutex *m1;
	struct thread_cv *cv;
	if(argptr(0,(char**)&cv,sizeof(*cv)) < 0 || argptr(1,(char**)&m1,sizeof(*m1)) < 0)
	return -1;
	//cprintf("In sysproc.c in cond_wait with args cv->channel: %d\n",cv->channel);
	thread_cond_wait(cv,m1); 
	return 0;
}

int sys_thread_cond_signal(void)
{
	//cprintf("In sysproc.c cond_signal\n");
	struct thread_cv *cv;
	if(argptr(0,(char**)&cv,sizeof(*cv)) < 0)
	return -1;
	//thread_cond_signal(cv);
	wakeup((void*)cv->channel);
	return 0;
}

// Spinlock implementation

int sys_thread_spin_init(void)
{
	//cprintf("sys_thread_spin_init\n");
	struct thread_spinlock *lock1;
	if(argptr(0,(char**)&lock1,sizeof(*lock1)) < 0)
		return -1;
	return thread_spin_init(lock1);
}

int sys_thread_spin_lock(void)
{
	struct thread_spinlock *lock1;
        if(argptr(0,(char**)&lock1,sizeof(*lock1)) < 0)
                return -1;
        return thread_spin_lock(lock1);

}

int sys_thread_spin_unlock(void)
{
	struct thread_spinlock *lock1;
        if(argptr(0,(char**)&lock1,sizeof(*lock1)) < 0)
                return -1;
        return thread_spin_unlock(lock1);
}


