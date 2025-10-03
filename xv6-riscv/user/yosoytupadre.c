#include "kernel/types.h"
#include "user.h"

int main(void)
{

 // prueba syscall getppid()

  int pid_hijo;

  pid_hijo = fork();

  switch(pid_hijo) {
    case -1:
      // Error al crear el hijo
      printf("No se pudo crear el proceso hijo\n");
      exit(1);

    case 0:
      // Código del hijo
      printf("Soy el hijo -> PID: %d | PID de mi padre: %d\n", getpid(), getppid());
      exit(0);

    default:
      // Código del padre
      wait(0); // Espera a que el hijo termine
      printf("Soy el padre -> PID: %d | PID de mi hijo: %d\n", getpid(), pid_hijo);
  }

  // prueba syscall getancestor()

  printf("\nProbando Syscall getancestor:\n");

  printf("ID proceso : %d\n", getancestor(0));
  printf("el ID del padre es : %d\n", getancestor(1));
  printf("el ID del abuelo es : %d\n", getancestor(2));
  if (getancestor(3)>-1){
       printf(" el ID del bisabuelo es : %d\n", getancestor(3));
    } else {
      printf("no existe bisabuelo del proceso: %d\n", getancestor(0));
    }
  exit(0);
}