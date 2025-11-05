# Trabajo Práctico Especial - Arquitectura de Computadoras - 1C 2025

## Integrantes
- Joaquin Pelufo 63623
- Jeronimo Di Leo Muntaner 63051
- Segundo MArtinez Tanoira 62228

---

## Manual de usuario

### Requerimientos
- Tener instalado el programa `make`
- Tener instalado el programa `gcc`
- Tener instalado el programa `nasm`
- Tener instalado el programa `docker`
- Tener instalado el programa `qemu`

---

## Compilación

### 1. Descargar la imagen de Docker
```bash
docker pull agodio/itba-so:latest
```

### 2. Compilar dentro del contenedor Docker
```bash
# Montar el volumen y entrar al contenedor
docker run -v ${PWD}:/root/TPE --security-opt seccomp:unconfined -ti agodio/itba-so:latest

# Dentro del contenedor:
cd /root/TPE/Toolchain
make all

cd /root/TPE
make all

# Salir del contenedor
exit
```

---

## Ejecución

### Método recomendado
```bash
./run.sh
```

### Método alternativo (desde Image/)
```bash
cd Image
make run
```

### Ejecutar con audio (macOS)
Editar `run.sh` y descomentar la línea para macOS (coreaudio).

---

## Uso de Shell

Es posible utilizar la shell como un intérprete de comandos. Para ejecutar un comando, simplemente escribirlo y presionar la tecla **ENTER**.

### Comandos disponibles:

| Comando | Descripción | Parámetros |
|----------|--------------|------------|
| `help` | Muestra un mensaje de ayuda con los comandos disponibles | - |
| `clear` | Limpia la pantalla | - |
| `exit` | Cierra la shell terminando con la ejecución | - |
| `time` | Muestra la fecha y hora actual | - |
| `timeZone` | Modifica el UTC de `time` | - |
| `scale <1-3>` | Cambia el tamaño de la fuente de la shell | 1, 2 o 3 |
| `inforeg` | Muestra los valores de los registros (guardar con Ctrl+R) | - |
| `divzero` | Prueba la excepción de división por cero | - |
| `invopcode` | Prueba la excepción de código de operación inválido | - |
| `beep <freq> <ms>` | Reproduce un sonido a través del PC Speaker | Frecuencia (Hz), duración (ms) |
| `tron <fps>` | Ejecuta el juego TRON | Opcional: `fps` muestra los cuadros por segundo en pantalla |
| `benchmark <param>` | Ejecuta pruebas de rendimiento del sistema.<br><br>**Parámetros disponibles:**<br>• `cpu` → Mide rendimiento del procesador mediante operaciones con enteros y punto flotante.<br>• `mem` → Evalúa la velocidad de lectura y escritura en memoria.<br>• `sys` → Mide el tiempo de ejecución promedio de llamadas al sistema (syscalls).<br>• `io` → Evalúa el rendimiento de operaciones de entrada/salida con los drivers.<br><br>Si se ejecuta sin parámetros, corre **todas las pruebas** de forma secuencial. |


### Juego TRON
- **Jugador 1:** Teclas `W`, `A`, `S`, `D`
- **Jugador 2:** Teclas `I`, `J`, `K`, `L`
- **Reiniciar partida:** `r`
- **Salir:** `ESC`

---

## System Calls

A continuación se detallan las system calls implementadas y sus respectivos parámetros:

| Nombre | %rax | %rdi | %rsi | %rdx | %rcx | %r8 |
|--------|------|------|------|------|------|-----|
| `read` | 0 (SYS_READ) | int fd | char * buffer | uint64_t size | - | - |
| `write` | 1 (SYS_WRITE) | int fd | const char * buffer | uint64_t size | uint64_t color | - |
| `drawRectangle` | 2 (SYS_RECTANGLE) | uint32_t hexColor | uint64_t x | uint64_t y | uint64_t width | uint64_t height |
| `clearScreen` | 3 (SYS_CLEAR) | - | - | - | - | - |
| `getCoords` | 4 (SYS_COORDS) | - | - | - | - | - |
| `getScreenInfo` | 5 (SYS_SCREENINFO) | - | - | - | - | - |
| `getFontInfo` | 6 (SYS_FONTINFO) | - | - | - | - | - |
| `getTime` | 7 (SYS_GETTIME) | uint64_t arg | - | - | - | - |
| `setFontScale` | 8 (SYS_SETSCALE) | uint64_t newScale | - | - | - | - |
| `getRegisters` | 9 (SYS_GETREGS) | uint64_t * regs | - | - | - | - |
| `sleep` | 10 (SYS_SLEEP) | uint64_t millis | - | - | - | - |
| `playSound` | 11 (SYS_PLAYSOUND) | uint64_t freq | uint64_t millis | - | - | - |
| `setBgColor` | 12 (SYS_SETBGCOLOR) | uint32_t newColor | - | - | - | - |
| `getBgColor` | 13 (SYS_GETBGCOLOR) | - | - | - | - | - |
| `exit` | 14 (SYS_EXIT) | int exitCode | - | - | - | - |
| `waitKey` | 15 (SYS_WAITKEY) | - | - | - | - | - |
| `getTicks` | 16 (SYS_GETTICKS) | - | - | - | - | - |

### Explicaciones de las system calls:

- **read:** Lee `size` bytes del file descriptor `fd` y los guarda en el buffer `buffer`. Retorna la cantidad de bytes leídos.
- **write:** Escribe `size` bytes del buffer `buffer` en el file descriptor `fd` con el color especificado en `color`. Retorna la cantidad de bytes escritos.
- **drawRectangle:** Dibuja un rectángulo sólido en la pantalla con el color `hexColor`, en la posición `(x, y)` y con las dimensiones `width` x `height`.
- **clearScreen:** Limpia la pantalla completa con el color de fondo configurado actualmente.
- **getCoords:** Obtiene las coordenadas actuales del cursor. Devuelve por `rax` un valor de 64 bits donde los primeros 32 bits representan la coordenada x y los siguientes 32 bits representan la coordenada y.
- **getScreenInfo:** Obtiene la información de la pantalla. Devuelve por `rax` un valor de 64 bits donde los primeros 32 bits representan el ancho en píxeles y los siguientes 32 bits representan el alto en píxeles.
- **getFontInfo:** Obtiene la información de la fuente actual. Devuelve por `rax` un valor de 64 bits donde los primeros 32 bits representan el ancho de la fuente y los siguientes 32 bits representan el alto de la fuente.
- **getTime:** Lee los datos de fecha y hora del RTC (Real-Time Clock). El parámetro `arg` indica qué componente temporal leer (segundos, minutos, horas, día, mes, año).
- **setFontScale:** Cambia la escala de la fuente de texto (valores válidos: 1, 2, 3). Retorna 1 si la escala es válida, 0 en caso contrario.
- **getRegisters:** Obtiene los valores de los 17 registros (RAX-R15, RIP, RSP) y los guarda en el array `regs`. Antes de ejecutar esta syscall se deben capturar los valores de los registros utilizando la combinación de teclas **Ctrl+R**.
- **sleep:** Pausa la ejecución del programa actual por `millis` milisegundos.
- **playSound:** Reproduce un sonido a través del PC Speaker con frecuencia `freq` (en Hz) durante `millis` milisegundos.
- **setBgColor:** Cambia el color de fondo de la pantalla al color especificado en `newColor` (formato 0xRRGGBB).
- **getBgColor:** Obtiene el color de fondo actual de la pantalla. Devuelve el valor en `rax` (formato 0xRRGGBB).
- **exit:** Termina la ejecución del programa de usuario y retorna al shell con el código de salida `exitCode` (0 = éxito, otro = error).
- **waitKey:** Espera bloqueante hasta que el usuario presione una tecla. Retorna el scancode de la tecla presionada.
- **getTicks:** Obtiene el contador de ticks del sistema (cantidad de interrupciones de timer desde el arranque). Útil para medir tiempo y rendimiento.

---

## Características del sistema

### Video Driver
- Resolución: Detectada automáticamente vía VBE Mode Info
- Soporte para 24 bpp y 32 bpp
- Orden de color configurable (BGR/RGB)
- Fuente bitmap 8x16 píxeles
- Escalado de texto: 1x, 2x, 3x
- Clipping automático (no dibuja fuera de pantalla)

### Teclado
- Driver de teclado con buffer circular
- Soporte para teclas especiales (Ctrl, Shift, Alt)
- Combinación Ctrl+R para capturar registros

### Audio
- Reproducción de sonidos mediante PC Speaker
- Frecuencias configurables (20 Hz - 20 kHz)
- Duración configurable

### Excepciones
- División por cero (Exception #0)
- Código de operación inválido (Exception #6)
- Snapshots de registros en el momento de la excepción

---

## Estructura del proyecto

```
TPE/
├── Bootloader/       # Pure64 bootloader
├── Kernel/           # Código del kernel
│   ├── asm/          # Código assembly
│   ├── drivers/      # Drivers (video, teclado, audio)
│   ├── idt/          # Manejo de interrupciones y syscalls
│   └── include/      # Headers
├── Userland/         # Código de usuario
│   └── SampleCodeModule/
│       ├── asm/      # Syscalls en assembly
│       ├── shell/    # Implementación de la shell
│       └── includes/ # Headers de userland
├── Image/            # Imagen booteable
└── Toolchain/        # Herramientas de compilación
```

---

## Licencia
Proyecto académico de Arquitectura de Computadoras - ITBA 2025

