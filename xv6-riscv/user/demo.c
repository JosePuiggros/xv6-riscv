// En user/demo.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N_PROCESOS 10
#define ITERACIONES 5000000

int main(void) {
  printf("\n=== Test Lottery Scheduling ===\n");
  printf("Creando %d procesos con diferentes tickets...\n\n", N_PROCESOS);
  
  for (int i = 0; i < N_PROCESOS; i++) {
    int pid = fork();
    if (pid == 0) {
      // Proceso hijo
      int mis_tickets = 50 * (i + 1);
      
      if(settickets(mis_tickets) < 0) {
        printf("ERROR: settickets fallo\n");
        exit(1);
      }
      
      // Trabajo CPU-intensivo
      volatile long counter = 0;
      for(long j = 0; j < ITERACIONES; j++) {
        counter++;
      }
      
      // Al terminar, imprimir resultado
      printf("[HIJO %d] PID=%d Tickets=%d TERMINADO\n", i, getpid(), mis_tickets);
      exit(0);
    }
  }
  
  // Padre espera a todos los hijos
  for(int i = 0; i < N_PROCESOS; i++) {
    wait(0);
  }
  
  printf("\n=== Test completado exitosamente ===\n\n");
  exit(0);
}
