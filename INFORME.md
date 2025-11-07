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
### 3) Selección de proceso (lotería)

