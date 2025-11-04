#include "benchmark.h"
#include "syscalls.h"
#include "io_utils.h"
#include "string_utils.h"
#include "time.h"
#include <stdint.h>

// ============== CONSTANTES ==============

#define MINOR_WAIT 50000000       // 50 millones - benchmark rápido (~1 segundo)
#define MEDIUM_WAIT 200000000     // 200 millones - benchmark medio (~4 segundos)
#define ARRAY_SIZE 10000          // Tamaño del array para test de memoria
#define MEM_ITERATIONS 1000       // Iteraciones del test de memoria
#define SYSCALL_ITERATIONS 100000 // Cantidad de syscalls en test
#define IO_ITERATIONS 10000       // Iteraciones del test de I/O

// ============== FUNCIONES DE BENCHMARKING ==============


//Función de prueba para medir rendimiento de CPU
static uint64_t cpu_intensive_operation(uint64_t n) {
    uint64_t result = 0;
    for (uint64_t i = 0; i < n; i++) {
        result += i * i;
        result = result % 1000000;
    }
    return result;
}


//Benchmark de operaciones con enteros
static void benchmark_integer_ops(uint64_t iterations) {
    puts("  [INT] Operaciones con enteros... ", COLOR_WHITE);
    
    uint64_t start_time = getTicks();
    uint64_t result = cpu_intensive_operation(iterations);
    uint64_t end_time = getTicks();
    
    uint64_t elapsed = end_time - start_time;
    
    char buffer[32];
    itoa(elapsed, buffer, 10);
    puts(buffer, COLOR_CYAN);
    puts(" ticks (resultado: ", COLOR_WHITE);
    itoa(result, buffer, 10);
    puts(buffer, COLOR_CYAN);
    puts(")\n", COLOR_WHITE);
}

static volatile double float_result_sink = 0.0;
//Benchmark de operaciones con punto flotante
static void benchmark_float_ops(uint64_t iterations) {
    puts("  [FPU] Operaciones con punto flotante... ", COLOR_WHITE);
    
    uint64_t start_time = getTicks();
    
    double result = 0.0;
    for (uint64_t i = 0; i < iterations; i++) {
        result += (double)i * 1.5;
        result = result * 0.9999;
    }
    
    uint64_t end_time = getTicks();
    uint64_t elapsed = end_time - start_time;
    
    char buffer[32];
    itoa(elapsed, buffer, 10);
    puts(buffer, COLOR_CYAN);
     puts(" ticks (resultado: ", COLOR_WHITE);

    int is_negative = result < 0.0;
    double abs_result = is_negative ? -result : result;
    uint64_t integer_part = (uint64_t)abs_result;
    double fractional_value = (abs_result - (double)integer_part) * 10000.0;
    if (fractional_value < 0) {
        fractional_value = 0;
    }
    uint64_t fractional_part = (uint64_t)fractional_value;
    if (fractional_part > 9999) {
        fractional_part = 9999;
    }

    if (is_negative) {
        putcharColor('-', COLOR_CYAN);
    }

    itoa(integer_part, buffer, 10);
    puts(buffer, COLOR_CYAN);
    putcharColor('.', COLOR_CYAN);

    char fractional_buffer[5];
    for (int i = 3; i >= 0; i--) {
        fractional_buffer[i] = '0' + (fractional_part % 10);
        fractional_part /= 10;
    }
    fractional_buffer[4] = '\0';
    puts(fractional_buffer, COLOR_CYAN);
    puts(")\n", COLOR_WHITE);

    float_result_sink = result; // Evitar optimización
}

//Benchmark de acceso a memoria
static void benchmark_memory_access(void) {
    puts("  [MEM] Accesos a memoria... ", COLOR_WHITE);
    
    static uint64_t array[ARRAY_SIZE];
    
    uint64_t start_time = getTicks();
    
    uint64_t sum = 0;
    // Repetir el test múltiples veces para que sea medible
    for (int iter = 0; iter < MEM_ITERATIONS; iter++) {
        // Escribir en memoria
        for (int i = 0; i < ARRAY_SIZE; i++) {
            array[i] = i * 2;
        }
        
        // Leer de memoria
        for (int i = 0; i < ARRAY_SIZE; i++) {
            sum += array[i];
        }
    }
    
    uint64_t end_time = getTicks();
    uint64_t elapsed = end_time - start_time;
    
    char buffer[32];
    itoa(elapsed, buffer, 10);
    puts(buffer, COLOR_CYAN);
    puts(" ticks (suma: ", COLOR_WHITE);
    itoa(sum, buffer, 10);
    puts(buffer, COLOR_CYAN);
    puts(")\n", COLOR_WHITE);
}


//Benchmark de llamadas al sistema (syscalls)

static void benchmark_syscalls(void) {
    puts("  [SYS] Llamadas al sistema... ", COLOR_WHITE);
    
    uint64_t start_time = getTicks();
    
    for (int i = 0; i < SYSCALL_ITERATIONS; i++) {
        sys_get_coords();  
    }
    
    uint64_t end_time = getTicks();
    uint64_t elapsed = end_time - start_time;
    
    char buffer[32];
    itoa(elapsed, buffer, 10);
    puts(buffer, COLOR_CYAN);
    puts(" ticks (", COLOR_WHITE);
    itoa(SYSCALL_ITERATIONS, buffer, 10);
    puts(buffer, COLOR_CYAN);
    puts(" llamadas)\n", COLOR_WHITE);
}


//Benchmark de operaciones de I/O

static void benchmark_io_operations(void) {
    puts("  [I/O] Operaciones de entrada/salida... ", COLOR_WHITE);
    
    uint64_t start_time = getTicks();
    
    for (int i = 0; i < IO_ITERATIONS; i++) {
        sys_get_screen_info();
        sys_get_font_info();
    }
    
    uint64_t end_time = getTicks();
    uint64_t elapsed = end_time - start_time;
    
    char buffer[32];
    itoa(elapsed, buffer, 10);
    puts(buffer, COLOR_CYAN);
    puts(" ticks (", COLOR_WHITE);
    itoa(IO_ITERATIONS * 2, buffer, 10);
    puts(buffer, COLOR_CYAN);
    puts(" operaciones)\n", COLOR_WHITE);
}


// Ejecuta todos los  benchmarks

void run_benchmarks(void) {
    puts("\n", COLOR_WHITE);
    puts("╔══════════════════════════════════════╗\n", COLOR_CYAN);
    puts("║        SISTEMA DE BENCHMARKING       ║\n", COLOR_CYAN);
    puts("╚══════════════════════════════════════╝\n", COLOR_CYAN);
    puts("\n", COLOR_WHITE);
    
    puts("Ejecutando benchmarks...\n\n", COLOR_WHITE);
    
    // Benchmarks de CPU
    puts("━━━ RENDIMIENTO DE CPU ━━━\n", COLOR_YELLOW);
    benchmark_integer_ops(MINOR_WAIT);
    benchmark_float_ops(MINOR_WAIT / 10);
    
    puts("\n", COLOR_WHITE);
    
    // Benchmarks de memoria
    puts("━━━ ACCESO A MEMORIA ━━━\n", COLOR_YELLOW);
    benchmark_memory_access();
    
    puts("\n", COLOR_WHITE);
    
    // Benchmarks de sistema
    puts("━━━ LLAMADAS AL SISTEMA ━━━\n", COLOR_YELLOW);
    benchmark_syscalls();
    benchmark_io_operations();
    
    puts("\n", COLOR_WHITE);
    puts("═══════════════════════════════════════\n", COLOR_GREEN);
    puts("Benchmarks completados!\n", COLOR_GREEN);
    puts("═══════════════════════════════════════\n", COLOR_GREEN);
    puts("\n", COLOR_WHITE);
}


 //Ejecuta un benchmark específico

void run_specific_benchmark(const char *test_name) {
    if (strcmp(test_name, "cpu") == 0) {
        puts("\n━━━ BENCHMARK DE CPU ━━━\n", COLOR_YELLOW);
        benchmark_integer_ops(MEDIUM_WAIT);
        benchmark_float_ops(MEDIUM_WAIT / 10);
    } else if (strcmp(test_name, "mem") == 0 || strcmp(test_name, "memory") == 0) {
        puts("\n━━━ BENCHMARK DE MEMORIA ━━━\n", COLOR_YELLOW);
        benchmark_memory_access();
    } else if (strcmp(test_name, "sys") == 0 || strcmp(test_name, "syscall") == 0) {
        puts("\n━━━ BENCHMARK DE SYSCALLS ━━━\n", COLOR_YELLOW);
        benchmark_syscalls();
    } else if (strcmp(test_name, "io") == 0) {
        puts("\n━━━ BENCHMARK DE I/O ━━━\n", COLOR_YELLOW);
        benchmark_io_operations();
    } else {
        puts("Error: Test no reconocido.\n", COLOR_RED);
        puts("Tests disponibles: cpu, mem, sys, io\n", COLOR_WHITE);
    }
}


//Muestra información de ayuda sobre benchmarks

void benchmark_help(void) {
    puts("\n", COLOR_WHITE);
    puts("BENCHMARK - Sistema de medicion de rendimiento\n\n", COLOR_CYAN);
    puts("Uso:\n", COLOR_WHITE);
    puts("  benchmark          - Ejecuta todos los benchmarks\n", COLOR_WHITE);
    puts("  benchmark <test>   - Ejecuta un benchmark especifico\n", COLOR_WHITE);
    puts("\n", COLOR_WHITE);
    puts("Tests disponibles:\n", COLOR_YELLOW);
    puts("  cpu     - Operaciones de CPU (enteros y flotantes)\n", COLOR_WHITE);
    puts("  mem     - Acceso a memoria (lectura/escritura)\n", COLOR_WHITE);
    puts("  sys     - Llamadas al sistema (syscalls)\n", COLOR_WHITE);
    puts("  io      - Operaciones de entrada/salida\n", COLOR_WHITE);
    puts("\n", COLOR_WHITE);
}
