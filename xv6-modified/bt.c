#include "types.h"
#include "stat.h"
#include "user.h"

int baz() __attribute__((noinline)); 
int baz() {
  // printf(1,"%p\n", __builtin_return_address(0));
   int a; 
   a = backtrace();
   return a + uptime();  
}

int bar() __attribute__((noinline)); 
int bar() {
  //printf(1,"%p\n", __builtin_return_address(0));
  int b; 
  b = baz(); 
  return b + uptime();
}

int foo() __attribute__((noinline)); 
int foo() {
  //printf(1,"%p\n", __builtin_return_address(0));
  int c; 
  c = bar();
  return c + uptime();
}


int
main(int argc, char *argv[])
{
  //printf(1,"%p\n", __builtin_return_address(0));
  foo();
  exit();
}
