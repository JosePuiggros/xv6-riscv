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
#define SYS_getppid 22
#define SYS_getancestror 23
```

### 3) añadir ambas funciones a todos los registro necesarios para que puedan llamarse y funcionar correctamente.

3.1) en el archivo syscall.c, debajo de la linea 82 : "// Prototypes for the functions that handle system calls."
añadimos las 2 creadas con anterioridad

```bash
extern uint64 sys_getppid(void);  
extern uint64 sys_getancestror(void);
```
3.2) en el archivo sysproc.c se deben añadir para agregarlas a una lista de punteros hacia funciones (xv6 recorre esta lista para acceder a ellas).
```bash
[SYS_getppid] sys_getppid,   
[SYS_getancestror] sys_getancestror,
```
3.3) esta vez saldremos del kernel y en la carpeta user se debe agregar informacion en user.h (igualmente para el correcto funcionamiento e instanciacion de las funciones)
```bash
int getppid(void);
int getancestror(int);
```
3.4) manteniendonos en la carpeta user, el archivo usys.pl tambien se debe editar, añadiendo lo siguiente
```bash
entry("getppid");
entry("getancestror"); 
```

### 4) una vez configuradas las funciones se procede a probarlas.

se crea el archivo yosoytupadre.c en la carpeta user siguiendo las instrucciones de la pauta, luego se utiliza el codigo visto en clases para implementar las 2 nuevas system calls:

```bash
cofdigooooooooooooo
```
### 5) Posteriormente se replica lo mismo con la otra funcion -----> getancestor()

```bash
```
### 6) manejo de fallo!!!

al querer correr el script vemos que no aparece entre las opciones al ejecutar un "ls", esto se debia a que los 2 nuevos archivos no estaban inlcuidos en el Makefile, por lo tanto no se estaban compilando en el ejecutable.

```c
$ yosoytupadre
exec yosoytupadre failed
```
se soluciono añadiendo lo siguiente en el makefile:
```bash
$U/_yosoytupadre\
$U/_test_ancestror\
```


### CONFIRMACION DE USO:
<img width="649" height="335" alt="image" src="https://github.com/user-attachments/assets/afea1423-25b0-4450-845f-ba1f795928dc" />
