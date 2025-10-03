# Tarea 1 - Implementación de Llamadas al Sistema en xv6

Jose Fritz & Jose Puiggros 

### 1) creamos los 2 nuevos sistem calls -----> getppid() & getancestor()

para crear nuevos sistem calls en el SO de xv6 debemos acceder al kernel y añadir lo siguiente:

#### getppid()
```bash
aca va el codigo del sys_getppid()
```
#### getancestor()
```bash
aca va el codigo del sys_getancestor()
```
esto se hace de la siguiente manera, ya que el SO debe tener en sus registros las sistem calls para poder utilizarlas posteriormente en los procesos.

### 2) asignar ID a las system calls

como todo proceso, los dos nuevos deben tener un numero identificador para poder ser detectados y utilizados. Esto se hace en el archivo syscall.h agregando las siguentes lineas 

```bash
codigo correspondiente
```

### 3) añadir ambas funciones al registro de funciones que manejan llamadas de sistema

en el archivo syscall.c, debajo de la linea 82 : "// Prototypes for the functions that handle system calls."
añadimos las 2 creadas con anterioridad

```bash
extern uint64 sys_getppid(void);  
extern uint64 sys_getancestror(void);
```
