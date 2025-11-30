# Tarea 3: Protección de Lectura en XV6

Jose Fritz & Jose Puiggros

---

## 1) Agregar los 2 nuevos syscalls en todos los archivos correspondientes

### user.h

```bash
int mrdprotect(void *addr, int len);
int munrdprotect(void *addr, int len);
```

### usys.pl

```bash
entry("mrdprotect");
entry("munrdprotect");
```

### usys.S

```bash
mrdprotect:
    li a7, SYS_mrdprotect
    ecall
    ret
//--------------------
munrdprotect:
    li a7, SYS_munrdprotect
    ecall
    ret
```

### syscall.h

```bash
#define SYS_mrdprotect   25
#define SYS_munrdprotect 26
```

### syscall.c

```bash
extern uint64 sys_mrdprotect(void);
extern uint64 sys_munrdprotect(void);
```

Dentro del mismo archivo, debemos agregarlo a una lista de syscalls[]:

```bash
[SYS_mrdprotect]  sys_mrdprotect,
[SYS_munrdprotect] sys_munrdprotect,
```

### defs.h

```bash
int mrdprotect(void *addr, int len);
int munrdprotect(void *addr, int len);
......
uint64 sys_mrdprotect(void);
uint64 sys_munrdprotect(void);
```

---

## 2) ahora creamos las 2 funciones dentro del archivo vm.c

### mrdprotect()

```bash
// Implementación de mrdprotect
int
mrdprotect(uint64 addr, uint64 len)
{
  pte_t pte;
  uint64 a;

  // 1. Validaciones básicas [cite: 31]
  if(len <= 0 || (addr % PGSIZE) != 0) 
    return -1;

  // 2. Recorrer el rango de páginas [cite: 18, 19]
  for(a = addr; a < addr + len PGSIZE; a += PGSIZE){
    // Verificar que la dirección sea parte del espacio de usuario [cite: 32]
    if(a >= MAXVA) 
        return -1;

    // Buscar la entrada en la tabla de páginas (walk)
    // myproc()->pagetable es la tabla del proceso actual
    pte = walk(myproc()->pagetable, a, 0);

    // 3. Verificar validez y permisos [cite: 21, 33]
    if(pte == 0  (*pte & PTE_V) == 0  (*pte & PTE_U) == 0)
      return -1;

    // 4. Modificar bit: Limpiar PTE_R [cite: 14, 18, 21]
    pte &= ~PTE_R; 
  }

  // Refrescar TLB (sfence.vma) para aplicar cambios inmediatamente
  sfence_vma(); 
  return 0;
}
```

Esta función recorre len páginas comenzando en la dirección addr y elimina el permiso de lectura en cada una, modificando sus PTEs para que cualquier intento de lectura sobre ese rango de memoria produzca un page fault.

---

### munrdprotect()

```bash
// Implementación de munrdprotect
int
munrdprotect(uint64 addr, uint64 len)
{
  pte_tpte;
  uint64 a;

  if(len <= 0  (addr % PGSIZE) != 0) 
    return -1;

  for(a = addr; a < addr + len * PGSIZE; a += PGSIZE){
    if(a >= MAXVA) return -1;

    pte = walk(myproc()->pagetable, a, 0);

    // Verificar validez [cite: 25]
    if(pte == 0  (pte & PTE_V) == 0 || (pte & PTE_U) == 0)
      return -1;

    // Restaurar bit: Activar PTE_R [cite: 15, 24]
    *pte |= PTE_R;
  }

  sfence_vma();
  return 0;
}
```

Esta función realiza la operación inversa: recorre len páginas desde addr y restaura el permiso de lectura en sus PTEs, permitiendo nuevamente que el proceso lea desde ese rango de memoria.

---

## 3) Luego debemos agregar esas funciones dentro de sysproc.c para conectar las llamadas que hace un programa en espacio de usuario con la función real del kernel que implementa la lógica.

### sysproc.c

```bash
uint64
sys_mrdprotect(void)
{
  uint64 addr;
  int len;

  // Obtener argumentos (dirección y longitud)
  if(argaddr(0, &addr) < 0  argint(1, &len) < 0)
    return -1;

  return mrdprotect(addr, len);
}

uint64
sys_munrdprotect(void)
{
  uint64 addr;
  int len;

  if(argaddr(0, &addr) < 0  argint(1, &len) < 0)
    return -1;

  return munrdprotect(addr, len);
}
```

---

## 4) Prueba y ejecución

### 4.1) Para poder probar las nuevas funcionalidades creamos el archivo rdprotect_test.c y se le agregó el siguiente código:

* notar que no se utilizo el codigo dado en el pdf guia, ya que se le tuvo que hacer una correción explicada en el punto 5.1)

```bash
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
  // Reservar una página (4096 bytes)
  char *addr = sbrk(4096); 

  // Escribir valor inicial (antes de proteger)
  addr[0] = 'Z';

  // Proteger contra lectura
  if (mrdprotect(addr, 1) < 0) {
    printf("mrdprotect falló\n");
    exit(1);
  }

  // Modificación para evitar error de hardware 0xf
  // Comentamos la escritura porque en RISC-V Write-Only es inválido.
  // addr[0] = 'A'; 

  // Intento de lectura: debería fallar
  // Si mrdprotect funciona, esto causará un TRAP (scause 0xd)
  char c = addr[0]; 

  // Si llegamos aquí, la protección NO funcionó
  printf("Valor leído: %c (esto NO debería imprimirse)\n", c);

  // Revertir protección
  if (munrdprotect(addr, 1) < 0) {
    printf("munrdprotect falló\n");
    exit(1);
  }

  printf("Protección revertida correctamente.\n");
  exit(0);
}
```

### 4.2) luego se agregó la siguiente línea al makefile para incluirlo en la compilación del QEMU:

```bash
$U/_rdprotect_test\ debajo de UPROGS
```

### 4.3) validación de la ejecución
<img width="472" height="189" alt="image" src="https://github.com/user-attachments/assets/0ab737fc-2ad1-4df0-ad66-7de2bf0e5cbb" />

- scause 0xd (13 en decimal): En la arquitectura RISC-V, el código de excepción 13 significa Load Page Fault (Fallo de página por carga/lectura).

- La Causa: El procesador intentó ejecutar la línea char c = addr[0]; (leer memoria).


- El Resultado: Como tu función mrdprotect eliminó exitosamente el bit PTE_R (permiso de lectura), la CPU bloqueó la operación y mató el proceso.

#### Conclusión: Has implementado con éxito una región de memoria "Write-Only" (o al menos, protegida contra lectura). El sistema operativo protegió el secreto tal como pedía el enunciado.
---


## 5) Aprendizaje

### 5.1) Dificultades en el proceso:

* A lo largo del desarrollo surgieron dificultades relacionadas principalmente con el comportamiento real del hardware RISC-V, que no estaban consideradas en un principio. Aunque la manipulación de bits en las PTE, como limpiar el permiso de lectura y luego restaurarlo, estaba correctamente implementada y la lógica general era sólida, el procesador RISC-V impone una restricción importante: no permite páginas con permiso de escritura sin permiso de lectura. Esto significa que una página marcada como Write-Only (W=1, R=0), tal como pedía el enunciado, es en realidad una combinación inválida según la especificación del hardware, y provoca de inmediato un Store Page Fault al intentar escribir en ella. Debido a esto, el test fallaba incluso antes de llegar a la parte que buscaba generar un Read Fault.


