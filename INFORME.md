# Tarea 1 - Implementación de Llamadas al Sistema en xv6

Jose Fritz & Jose Puiggros 

### 1) creamos los 2 nuevos sistem calls -----> getppid() & getancestor()

para crear nuevos sistem calls en el SO de xv6 debemos acceder al kernel y añadir lo siguiente:

#### getppid()
```bash
uint64
sys_getppid(void)
{
  struct proc *p = myproc();  // Obtener proceso actual
  if(p->parent)
    return p->parent->pid;    // Retornar PID del padre
  else
    return 0;                 // Si no hay padre, retornar 0
}
```
#### getancestor()
```bash
uint64
// System call para retornar el ancestro n-ésimo del proceso actual
sys_getancestor(void)
{
    int n;
    struct proc *p;

    // Obtener argumento n
    argint(0, &n);   // En tu versión, argint es void, no retorna valor

    if (n < 0)
        return -1;

    p = myproc();

    for (int i = 0; i < n; i++) {
        if (p->parent)
            p = p->parent;
        else
            return -1;  // No hay suficientes ancestros
    }

    return p->pid;
}
```
esto se hace de la siguiente manera, ya que el SO debe tener en sus registros las sistem calls para poder utilizarlas posteriormente en los procesos.

### 2) asignar ID a las system calls

como todo proceso, los dos nuevos deben tener un numero identificador para poder ser detectados y utilizados. Esto se hace en el archivo syscall.h agregando las siguentes lineas 

```bash
#define SYS_getppid 22
#define SYS_getancestor 23
```

### 3) añadir ambas funciones a todos los registro necesarios para que puedan llamarse y funcionar correctamente.

3.1) en el archivo syscall.c, debajo de la linea 82 : "// Prototypes for the functions that handle system calls."
añadimos las 2 creadas con anterioridad

```bash
extern uint64 sys_getppid(void);  
extern uint64 sys_getancestor(void);
```
3.2) en el archivo sysproc.c se deben añadir para agregarlas a una lista de punteros hacia funciones (xv6 recorre esta lista para acceder a ellas).
```bash
[SYS_getppid] sys_getppid,   
[SYS_getancestor] sys_getancestor,
```
3.3) esta vez saldremos del kernel y en la carpeta user se debe agregar informacion en user.h (igualmente para el correcto funcionamiento e instanciacion de las funciones)
```bash
int getppid(void);
int getancestor(int);
```
3.4) manteniendonos en la carpeta user, el archivo usys.pl tambien se debe editar, añadiendo lo siguiente
```bash
entry("getppid");
entry("getancestor"); 
```

### 4) una vez configuradas las funciones se procede a probarlas.

se crea el archivo yosoytupadre.c en la carpeta user siguiendo las instrucciones de la pauta, luego se utiliza el codigo visto en clases para implementar las 2 nuevas system calls:

yosoytupadre.c:
```bash
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

  printf("ID proceso : %d\n", getancestor(0));
  printf("el ID del padre es : %d\n", getancestor(1));
  printf("el ID del abuelo es : %d\n", getancestor(2));
  if (getancestor(3)>-1){
       printf(" el ID del bisabuelo es : %d\n", getancestor(3));
    } else {
      printf("no existe bisabuelo del proceso: %d\n", getancestor(0))
    }
  exit(0);
}
```
### 5) manejo de fallos

5.1) al querer correr el script vemos que no aparece entre las opciones al ejecutar un "ls", esto se debia a que los 2 nuevos archivos no estaban inlcuidos en el Makefile, por lo tanto no se estaban compilando en el ejecutable.

```c
$ yosoytupadre
exec yosoytupadre failed
```
se soluciono añadiendo lo siguiente en el makefile:
```bash
$U/_yosoytupadre\
```

5.2) Una complicacion menor fue la sintaxsis de C, pero con un poco de ayuda de copilot se hace bastante mas facil 
5.3) Otra complicación fue saber exactamente todos los archivos que se tenian que cambiar para poder hacer un syscall de manera correcta 
5.4) Cometimos errores al escribir el nombre de las funciones en los diferentes archivo, esto nos arrojo "undefined reference", lo que nos hizo volver a revisar cada uno para poner los nombres de manera correcta

### 6) Funcionamiento de las syscall
Una syscall permite que un programa solicite servicios al kernel de forma segura. El programa llama a una función de biblioteca que genera un trap al kernel, el cual identifica la syscall, ejecuta la función correspondiente y devuelve el resultado al programa. De esta manera, se puede interactuar con recursos protegidos sin acceder directamente al hardware.

### CONFIRMACION DE USO:
<img width="509" height="351" alt="image" src="https://github.com/user-attachments/assets/ed44ad67-abc1-4da2-8f51-7a8542eac3a9" />

