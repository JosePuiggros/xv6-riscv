## Pasos seguidos para instalar xv6
1. **Primero se tiene que preparar  la WSL para el trabajo**
```bash
- sudo apt-update
- sudo apt-upgrade
```
esto confirmara que nuestro enotrno de trabajo esta actualizado con las ultimas versiones de las herramientas a utilizar

2. **Instalar qemu y herramientas necesarias para la emulación**
```bash
- sudo apt install -y build-essential git qemu-system-x86 qemu-system-misc gdb-multiarch
```
estos son necesarios para la correcta utilizacion de xv6, qemu será el emulador a utilizar.

3. **Clonar el repositorio de xv6-riscv**
```bash
- git clone https://github.com/mit-pdos/xv6-riscv.git
```

con esto instalamos xv6 en su ultima version (riscv).

4. **Instalar paquetes necesarios para instalar RISC-V GNU Toolchain**

```bash
- sudo apt install -y autoconf automake autotools-dev curl python3 \
libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex \
texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev
```
terminamos de instalar herramientas necesarias para el correcto funcionamiento del compilador. Posteriormente con el comando make se compilaran las herramientas necesarias.

5. **Instalar el RISC-V GNU Toolchain (bare-metal)**
	
```bash  
- git clone https://github.com/riscv/riscv-gnu-toolchain.git
- cd riscv-gnu-toolchain
- ./configure --prefix=/opt/riscv
- make
```

6. **Agregar toolchain al PATH**
```bash
- export PATH=/opt/riscv/bin:$PATH
```
agregamos las herramientas descargadas anteriormente al xv6

7. **Compilar xv6**
```bash
- cd xv6-riscv
- make
```
con el make ejecutado anteiromente compliamos el kernel siguiendo las instrucciones del makefile.

8. **Ejecutar xv6 en QEMU**
```bash
- make qemu 
```
abrimos la terminal el qemu lista para utilizarse.

## Problemas encontrados y soluciones

**Problema:** `./configure: No such file or directory` al compilar el toolchain.  
**Solución:** Esto sucedió porque no se ejecutó `./configure` desde la raíz correcta. La solución fue ejecutar `./configure` dentro de la carpeta `riscv-gnu-toolchain`.

**Problema:** `riscv64-unknown-elf-gcc: command not found`.  
**Solución:** Se había compilado solo el toolchain para Linux (`riscv64-unknown-linux-gnu-*`). Es necesario compilar el **bare-metal toolchain** con `make` para obtener `riscv64-unknown-elf-gcc`.

**Problema:** `make: qemu-system-riscv64: No such file or directory`.  
**Solución:** QEMU con soporte RISC-V no estaba instalado. La solución es instalarlo con `sudo apt install qemu-system-misc`.

**Problema:** Solicitud de usuario y contraseña al hacer push.  
**Solución:** GitHub ya no permite contraseñas normales. La solución fue configurar **SSH** para autenticación sin contraseña.

## Confirmación de Funcionamiento:

<img width="534" height="976" alt="image" src="https://github.com/user-attachments/assets/5dcda738-520f-43d0-918a-892b3ad5c067" />
