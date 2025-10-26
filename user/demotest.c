#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int pid;
  
  printf("Test basico\n");
  
  pid = fork();
  
  if(pid == 0) {
    printf("Hijo: settickets(200)\n");
    settickets(200);
    printf("Hijo: getrunslices()\n");
    int s = getrunslices();
    printf("Hijo: slices=%d\n", s);
    exit(0);
  }
  
  wait(0);
  printf("OK\n");
  exit(0);
}
