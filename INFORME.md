# Tarea 2 - Planificación de Procesos en XV6


Jose Fritz & Jose Puiggros 

### 1) Campo tickets en cada proceso

Lo primero fue modificar la estructura del proceso.
En proc.h y proc.c respectivamente se añadieron los siguientes campos para manejar los tickets y los ciclos de ejecución:


```bash
struct proc {
 .
 .
 .
  int tickets;                 
  int run_slices; 
```
```bash
static struct proc*
allocproc(void)
{
 .
 .
 .
  p->tickets = 100;          
  p->run_slices = 0;
}
```
### 2) Después se agregaró una nueva syscall en sysproc.c que nos permite asignar tickets a un proceso
```bash
uint64
sys_settickets(void)
{
  int n;
  struct proc *p = myproc();

  // Obtener argumento
  argint(0, &n);

  // Validar: mínimo 1 ticket
  if(n < 1)
    n = 1;

  // Asignar tickets al proceso actual
  acquire(&p->lock);
  p->tickets = n;
  release(&p->lock);

  return 0;  // Retornar éxito
}
```

importante mencionar que para que la syscall funciones se agregaron las siguientes lineas en syscall.h
```bash
[SYS_settickets] sys_settickets,
```
```bash
extern uint64 sys_settickets(void);

```
tambien se agregó la siguiente funcion para poder crear numeros aleatorios (lo cual será necesario en el scheduler)
```bash
static unsigned long rand_next = 1;

int
rand(void)
{
  rand_next = rand_next * 1103515245 + 12345;
  return (unsigned int)(rand_next / 65536) % 32768;
}

void
srand(unsigned int seed)
{
  rand_next = seed;
}
```
### 3) Selección de proceso (lotería)
Finalmente, se implementó el nuevo scheduler de tipo Lottery.
Toda la lógica necesaria para calcular el total de tickets, generar el número aleatorio y seleccionar el proceso ganador se incorporó directamente dentro de la función scheduler en proc.c (notar que esta función ya incluye los requerimientos de contabilidad, monitoreo y robustez)

```bash
scheduler(void)
{
  struct proc p;
  struct cpuc = mycpu();

  c->proc = 0;
  for(;;){
    intr_on();

    int total_tickets = 0;
    struct proc *chosen = 0;

    // 1. Calcular total de tickets y asegurar mínimo 1 ticket
    for(p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      if(p->state == RUNNABLE) {
        // ROBUSTEZ: Asegurar que todo proceso tenga al menos 1 ticket
        if(p->tickets < 1) {
          p->tickets = 1;
        }
        total_tickets += p->tickets;
      }
      release(&p->lock);
    }

    // ROBUSTEZ: Si no hay tickets (no hay procesos RUNNABLE), continuar
    if(total_tickets == 0) {
      continue;  // No bloquear el scheduler
    }

    // 2. Generar número aleatorio
    int winner = (rand() % total_tickets) + 1;

    // 3. Seleccionar el proceso ganador
    int accumulated = 0;
    for(p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      if(p->state == RUNNABLE) {
        accumulated += p->tickets;
        if(accumulated >= winner && chosen == 0) {
          chosen = p;
          break;  // Encontrado el ganador
        }
      }
      release(&p->lock);
    }

    // 4. Ejecutar el proceso seleccionado
    if(chosen) {
      // CONTABILIDAD: Incrementar contador
      chosen->run_slices++;
      chosen->state = RUNNING;
      c->proc = chosen;

      // Comentar en producción, descomentar para debug:
      // printf("PID=%d Tickets=%d Slices=%d\n", 
      //        chosen->pid, chosen->tickets, chosen->run_slices);

      swtch(&c->context, &chosen->context);

      c->proc = 0;
      release(&chosen->lock);
    }
  }
}
```

### 4) Prueba y ejecucion del nuevo scheduler

#### 4.1) se creo un archivo demo.c en la carpeta user con el siguiente contenido:
```bash
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
```
#### 4.2) Compilacion y Ejecucion del SO
 se utilizaron los siguientes comandos:

 ```bash
make clean

make CPUS=1 quemu

 demo

```

#### 4.3) validacion de la ejecucion:

<img width="499" height="435" alt="image" src="https://github.com/user-attachments/assets/99e41992-0c70-4eba-88af-61a6d0e7b67c" />


