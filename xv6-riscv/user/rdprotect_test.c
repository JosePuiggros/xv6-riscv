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