#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  printf("Test: llamando settickets(200)\n");
  settickets(200);
  printf("OK: settickets funciono\n");
  
  printf("Test: llamando getrunslices()\n");
  int slices = getrunslices();
  printf("OK: getrunslices retorno %d\n", slices);
  
  exit(0);
}
