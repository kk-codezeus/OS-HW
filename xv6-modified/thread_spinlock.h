// Mutual exclusion spin locks.
#ifndef THREAD_SPINLCK_H
#define THREAD_SPINLOCK_H

struct thread_spinlock {
  uint locked;       // Is the lock held?

  char *name;
  //char name[16];	        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  uint pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
                     // };
};

#endif
