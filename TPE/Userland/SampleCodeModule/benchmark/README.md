# Sistema de Benchmarking - TPE Arquitectura de Computadoras

## Descripción General

Este sistema de benchmarking permite medir y analizar el rendimiento del sistema operativo en diferentes aspectos:

- **FPS (Frames Per Second)**: Simulación de rendimiento de juegos/gráficos
- **Operaciones de Punto Flotante**: Mide el rendimiento de cálculos con números decimales
- **Operaciones Enteras**: Mide el rendimiento de cálculos con números enteros
- **Acceso a Memoria**: Evalúa la velocidad de lectura/escritura en RAM
- **Overhead de Syscalls**: Mide el tiempo que toma realizar llamadas al sistema
- **Dibujado de Píxeles**: Evalúa el rendimiento del driver de video

## Arquitectura

### Componentes

```
Userland/SampleCodeModule/
├── benchmark/
│   └── benchmark.c          # Implementación de los benchmarks
├── includes/
│   └── benchmark.h          # API pública y estructuras
└── shell/
    └── commands.c           # Comando 'benchmark' integrado en la shell
```

### Syscalls Agregadas

Se agregaron 2 nuevas syscalls para soporte de timing preciso:

1. **sys_get_ticks()** (0x10)
   - Retorna el número de ticks del timer del sistema
   - Útil para mediciones de bajo nivel

2. **sys_get_ms()** (0x11)
   - Retorna el tiempo transcurrido en milisegundos
   - Basado en `ms_elapsed()` del kernel

## Uso

### Comando en Shell

```bash
# Ejecutar suite completa de benchmarks
benchmark

# Ejecutar benchmark específico
benchmark int       # Operaciones enteras
benchmark float     # Operaciones de punto flotante
benchmark mem       # Acceso a memoria
benchmark syscall   # Overhead de syscalls
benchmark fps       # Simulación de FPS
benchmark pixels    # Dibujado de píxeles

# Ver ayuda
benchmark help
```

### Ejemplo de Uso Programático

```c
#include "benchmark.h"

// Ejecutar benchmark de operaciones enteras
BenchmarkResult result = benchmark_int_operations(100000);
benchmark_print_result(&result);

// Crear una suite personalizada
BenchmarkResult results[10];
BenchmarkSuite suite;
benchmark_init_suite(&suite, "Mi Suite", 10);
suite.results = results;

// Agregar tests...
results[suite.test_count++] = benchmark_float_operations(50000);
results[suite.test_count++] = benchmark_memory_access(4, 1000);

// Imprimir resultados
benchmark_print_suite(&suite);
```

### Macros para Timing Rápido

```c
BENCHMARK_START();
// ... código a medir ...
BENCHMARK_END("Mi operación");
```

## Benchmarks Disponibles

### 1. Integer Operations
- **Iteraciones**: 100,000
- **Operaciones**: Suma, multiplicación, XOR, shifts
- **Métrica**: Operaciones por segundo

### 2. Float Operations
- **Iteraciones**: 50,000
- **Operaciones**: Suma, multiplicación, división
- **Métrica**: Operaciones por segundo

### 3. Memory Access
- **Tamaño**: 4 KB
- **Iteraciones**: 1,000
- **Operaciones**: Lectura y escritura secuencial
- **Métrica**: MB/s

### 4. Syscall Overhead
- **Iteraciones**: 10,000
- **Syscall usada**: `sys_get_coords()`
- **Métrica**: Syscalls por segundo

### 5. FPS Simulation
- **Frames**: 1,000
- **Trabajo**: 1,000 operaciones por frame
- **Métrica**: FPS (Frames por segundo)

### 6. Draw Pixels
- **Píxeles**: 1,000
- **Operación**: Dibujar rectángulos 1x1
- **Métrica**: Píxeles por segundo

## Resultados de Ejemplo

### QEMU
```
========================================
Benchmark: Integer Operations (Add/Mul/Shift)
========================================
Iteraciones:     100000
Tiempo total:    45 ms
Tiempo/iter:     0.000450 ms
Ops/sec:         2222222
Ticks (inicio):  818
Ticks (fin):     819
========================================
```

### Hardware Real (ejemplo)
```
========================================
Benchmark: Float Operations (Add/Mul/Div)
========================================
Iteraciones:     50000
Tiempo total:    78 ms
Tiempo/iter:     0.001560 ms
Ops/sec:         641025
Ticks (inicio):  1419
Ticks (fin):     1421
========================================
```

## Comparación entre Plataformas

El sistema permite comparar rendimiento en:

1. **QEMU**: Emulación por software
2. **VirtualBox**: Virtualización con aceleración
3. **Hardware Físico**: 
   - Máquina 1: [Especificar CPU, RAM]
   - Máquina 2: [Especificar CPU, RAM]

### Formato de Reporte

Para cada plataforma, documentar:

- **CPU**: Modelo y frecuencia
- **RAM**: Cantidad y tipo
- **Entorno**: QEMU/VirtualBox/Bare Metal
- **Fecha**: Cuando se realizó el test

```
=== QEMU ===
CPU: Emulado x86_64
RAM: 512 MB
Ops/sec (int): 2,222,222
Ops/sec (float): 641,025
MB/s (memory): 1,234.56
...

=== Hardware Físico - Dell XPS 15 ===
CPU: Intel i7-9750H @ 2.6GHz
RAM: 16 GB DDR4
Ops/sec (int): 15,000,000
Ops/sec (float): 8,500,000
MB/s (memory): 8,192.00
...
```

## Implementación Técnica

### Medición de Tiempo

```c
uint64_t start = benchmark_get_time_ms();
// ... código a medir ...
uint64_t end = benchmark_get_time_ms();
uint64_t elapsed = end - start;
```

### Warmup

Todos los benchmarks incluyen un período de "warmup" (100 iteraciones) antes de la medición real para estabilizar caché y branch prediction.

### Prevención de Optimizaciones

Se usa `volatile` para evitar que el compilador optimice el código de benchmark:

```c
volatile double x = 1.5;
volatile double acc = 0.0;
for (uint64_t i = 0; i < iterations; i++) {
    acc += x * 2.3;
}
```

## Limitaciones

1. **Timer Resolution**: La resolución es de ~55ms por tick
2. **Buffer Size**: Memory benchmark limitado a 4KB (stack)
3. **Sin malloc**: Todos los buffers son estáticos
4. **Single-threaded**: No hay pruebas de concurrencia

## Extensión

Para agregar un nuevo benchmark:

1. Agregar función en `benchmark.c`:
```c
BenchmarkResult benchmark_mi_test(uint64_t iterations) {
    BenchmarkResult result;
    result.type = BENCH_CUSTOM;
    result.name = "Mi Test";
    // ... implementación ...
    return result;
}
```

2. Agregar prototipo en `benchmark.h`

3. Agregar opción en `cmd_benchmark()` en `commands.c`

## Referencias

- [memtest86.com](https://www.memtest86.com/) - Inspiración para tests de memoria
- Intel Architecture Manual - Para timing preciso
- OSDEV Wiki - Información sobre timers x86

## Autores

- TPE Arquitectura de Computadoras - ITBA 2025
