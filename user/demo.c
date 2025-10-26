#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N 10

void
cpu_work(void)
{
  volatile int x;
  int i, j, k;
  
  x = 0;
  for(i = 0; i < 10000; i++) {
    for(j = 0; j < 3000; j++) {
      for(k = 0; k < 100; k++) {
        x = x + 1;
      }
    }
  }
}

int
main(void)
{
  int i;
  int pid;
  
  printf("\n========================================\n");
  printf("    LOTTERY SCHEDULER - DEMO\n");
  printf("========================================\n\n");
  
  printf("Configuracion:\n");
  for(i = 0; i < N; i++) {
    printf("  Hijo %d: %d tickets\n", i, 50 * (i + 1));
  }
  
  printf("\n========================================\n");
  printf("Iniciando %d procesos...\n", N);
  printf("========================================\n\n");
  
  sleep(20);
  
  for(i = 0; i < N; i++) {
    pid = fork();
    
    if(pid < 0) {
      printf("ERROR fork\n");
      exit(1);
    }
    
    if(pid == 0) {
      int my_tickets;
      int my_slices;
      
      my_tickets = 50 * (i + 1);
      settickets(my_tickets);
      
      
      cpu_work();
      
      my_slices = getrunslices();
      
      printf("[Hijo %d] %d tickets -> %d slices\n", 
             i, my_tickets, my_slices);
      
      exit(0);
    }
  }
  
  printf("Todos los procesos creados.\n");
  printf("Ejecutando... (presiona Ctrl-P para ver progreso)\n\n");
  
  for(i = 0; i < N; i++) {
    wait(0);
  }
  
  printf("\n========================================\n");
  printf("         RESULTADOS FINALES\n");
  printf("========================================\n");
  printf("Revisa arriba los slices de cada proceso.\n");
  printf("Proporcion esperada: mas tickets -> mas slices\n\n");
  
  exit(0);
}