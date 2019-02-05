#include "syscall.h"
#include "traps.h"

#define SYSCALL(name) \
  .globl name; \
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
SYSCALL(exit)
SYSCALL(wait)
SYSCALL(pipe)
SYSCALL(read)
SYSCALL(write)
SYSCALL(close)
SYSCALL(kill)
SYSCALL(exec)
SYSCALL(open)
SYSCALL(mknod)
SYSCALL(unlink)
SYSCALL(fstat)
SYSCALL(link)
SYSCALL(mkdir)
SYSCALL(chdir)
SYSCALL(dup)
SYSCALL(getpid)
SYSCALL(sbrk)
SYSCALL(sleep)
SYSCALL(uptime)
//Entry for backtrace and getprocinfo
SYSCALL(backtrace)
SYSCALL(getprocinfo)
//Entry for thread functionalities
SYSCALL(thread_create)
SYSCALL(thread_join)
SYSCALL(thread_exit)
//Entry for thread synchronisation
SYSCALL(thread_spin_init)
SYSCALL(thread_spin_lock)
SYSCALL(thread_spin_unlock)
//Entry for condition variables system calls
SYSCALL(thread_create_sender)
SYSCALL(thread_create_receiver)
SYSCALL(thread_cond_wait)
SYSCALL(thread_cond_signal)
