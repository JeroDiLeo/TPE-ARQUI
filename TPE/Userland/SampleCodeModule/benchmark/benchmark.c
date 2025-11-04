/*
 * =============================================================================
 * 📊 BENCHMARK SUITE
 * =============================================================================
 * Sistema de benchmarking para medir rendimiento del sistema operativo.
 * Permite medir FPS, operaciones de punto flotante, acceso a hardware, etc.
 * =============================================================================
 */

#include "../includes/benchmark.h"
#include "../includes/syscalls.h"
#include "../includes/io_utils.h"
#include <stddef.h>

// ============== FUNCIONES DE TIEMPO ==============

uint64_t benchmark_get_time_ms(void) {
    return sys_get_ms();
}

uint64_t benchmark_get_ticks(void) {
    return sys_get_ticks();
}

// ============== INICIALIZACIÓN Y LIMPIEZA ==============

void benchmark_init_suite(BenchmarkSuite *suite, const char *name, int max_tests) {
    suite->suite_name = name;
    suite->test_count = 0;
    suite->total_time_ms = 0;
    
    // Asignar memoria para resultados (usando stack, sin malloc)
    // El usuario debe proporcionar el array
    suite->results = NULL;
}

void benchmark_free_suite(BenchmarkSuite *suite) {
    // En nuestro caso, no necesitamos liberar memoria
    // ya que usamos arrays estáticos
    suite->results = NULL;
    suite->test_count = 0;
}

// ============== BENCHMARKS INDIVIDUALES ==============

BenchmarkResult benchmark_float_operations(uint64_t iterations) {
    BenchmarkResult result;
    result.type = BENCH_FLOAT_OPS;
    result.name = "Float Operations (Add/Mul/Div)";
    result.iterations = iterations;
    
    // Warmup
    volatile double warmup_result = 0.0;
    for (uint64_t i = 0; i < BENCHMARK_WARMUP_ITERATIONS; i++) {
        warmup_result += 1.5 * 2.3 / 1.1;
    }
    
    // Benchmark real
    uint64_t start = benchmark_get_time_ms();
    result.start_ticks = benchmark_get_ticks();
    
    volatile double x = 1.5;
    volatile double y = 2.3;
    volatile double z = 1.1;
    volatile double acc = 0.0;
    
    for (uint64_t i = 0; i < iterations; i++) {
        // Mix de operaciones de punto flotante
        acc += x * y;
        acc /= z;
        x += 0.1;
        y -= 0.05;
        z += 0.01;
    }
    
    result.end_ticks = benchmark_get_ticks();
    uint64_t end = benchmark_get_time_ms();
    
    result.total_time_ms = end - start;
    result.time_per_iteration = (double)result.total_time_ms / (double)iterations;
    result.operations_per_sec = (double)iterations / ((double)result.total_time_ms / 1000.0);
    
    // Prevenir optimización del compilador
    if (acc > 0) {
        result.operations_per_sec += 0;
    }
    
    return result;
}

BenchmarkResult benchmark_int_operations(uint64_t iterations) {
    BenchmarkResult result;
    result.type = BENCH_INT_OPS;
    result.name = "Integer Operations (Add/Mul/Shift)";
    result.iterations = iterations;
    
    // Warmup
    volatile uint64_t warmup_result = 0;
    for (uint64_t i = 0; i < BENCHMARK_WARMUP_ITERATIONS; i++) {
        warmup_result = (warmup_result + i) * 3 >> 1;
    }
    
    // Benchmark real
    uint64_t start = benchmark_get_time_ms();
    result.start_ticks = benchmark_get_ticks();
    
    volatile uint64_t acc = 0;
    volatile uint64_t x = 12345;
    
    for (uint64_t i = 0; i < iterations; i++) {
        acc += x * 7;
        acc ^= (x << 3);
        acc >>= 1;
        x = (x * 1103515245 + 12345) & 0x7fffffff; // LCG
    }
    
    result.end_ticks = benchmark_get_ticks();
    uint64_t end = benchmark_get_time_ms();
    
    result.total_time_ms = end - start;
    result.time_per_iteration = (double)result.total_time_ms / (double)iterations;
    result.operations_per_sec = (double)iterations / ((double)result.total_time_ms / 1000.0);
    
    // Prevenir optimización
    if (acc > 0) {
        result.operations_per_sec += 0;
    }
    
    return result;
}

BenchmarkResult benchmark_memory_access(uint64_t size_kb, uint64_t iterations) {
    BenchmarkResult result;
    result.type = BENCH_MEMORY_ACCESS;
    result.name = "Memory Access (Sequential Read/Write)";
    result.iterations = iterations;
    
    // Buffer estático para evitar usar malloc
    #define MAX_BUFFER_SIZE 4096
    static volatile uint8_t buffer[MAX_BUFFER_SIZE];
    uint64_t actual_size = (size_kb * 1024 < MAX_BUFFER_SIZE) ? size_kb * 1024 : MAX_BUFFER_SIZE;
    
    // Warmup
    for (uint64_t i = 0; i < actual_size; i++) {
        buffer[i] = (uint8_t)i;
    }
    
    // Benchmark real
    uint64_t start = benchmark_get_time_ms();
    result.start_ticks = benchmark_get_ticks();
    
    volatile uint64_t sum = 0;
    for (uint64_t iter = 0; iter < iterations; iter++) {
        // Escritura secuencial
        for (uint64_t i = 0; i < actual_size; i++) {
            buffer[i] = (uint8_t)(i ^ iter);
        }
        
        // Lectura secuencial
        for (uint64_t i = 0; i < actual_size; i++) {
            sum += buffer[i];
        }
    }
    
    result.end_ticks = benchmark_get_ticks();
    uint64_t end = benchmark_get_time_ms();
    
    result.total_time_ms = end - start;
    result.time_per_iteration = (double)result.total_time_ms / (double)iterations;
    
    // Calcular MB/s
    double total_bytes = (double)(actual_size * iterations * 2); // *2 por read+write
    double total_seconds = (double)result.total_time_ms / 1000.0;
    result.operations_per_sec = (total_bytes / (1024.0 * 1024.0)) / total_seconds; // MB/s
    
    // Prevenir optimización
    if (sum > 0) {
        result.operations_per_sec += 0;
    }
    
    return result;
}

BenchmarkResult benchmark_syscall_overhead(uint64_t iterations) {
    BenchmarkResult result;
    result.type = BENCH_SYSCALL_OVERHEAD;
    result.name = "Syscall Overhead (getCoords)";
    result.iterations = iterations;
    
    // Warmup
    for (uint64_t i = 0; i < BENCHMARK_WARMUP_ITERATIONS; i++) {
        sys_get_coords();
    }
    
    // Benchmark real
    uint64_t start = benchmark_get_time_ms();
    result.start_ticks = benchmark_get_ticks();
    
    volatile uint64_t dummy = 0;
    for (uint64_t i = 0; i < iterations; i++) {
        dummy ^= sys_get_coords();
    }
    
    result.end_ticks = benchmark_get_ticks();
    uint64_t end = benchmark_get_time_ms();
    
    result.total_time_ms = end - start;
    result.time_per_iteration = (double)result.total_time_ms / (double)iterations;
    result.operations_per_sec = (double)iterations / ((double)result.total_time_ms / 1000.0);
    
    // Prevenir optimización
    if (dummy > 0) {
        result.operations_per_sec += 0;
    }
    
    return result;
}

BenchmarkResult benchmark_draw_pixels(uint64_t pixel_count) {
    BenchmarkResult result;
    result.type = BENCH_DRAW_PIXELS;
    result.name = "Draw Pixels (1x1 rectangles)";
    result.iterations = pixel_count;
    
    uint64_t screen_info = sys_get_screen_info();
    uint64_t width = (screen_info >> 32) & 0xFFFFFFFF;
    uint64_t height = screen_info & 0xFFFFFFFF;
    
    // Warmup
    for (uint64_t i = 0; i < BENCHMARK_WARMUP_ITERATIONS; i++) {
        sys_draw_rectangle(0xFF0000, i % width, i % height, 1, 1);
    }
    
    // Benchmark real
    uint64_t start = benchmark_get_time_ms();
    result.start_ticks = benchmark_get_ticks();
    
    for (uint64_t i = 0; i < pixel_count; i++) {
        uint32_t color = 0xFF0000 | ((i * 137) & 0xFFFF);
        uint64_t x = (i * 7) % width;
        uint64_t y = (i * 11) % height;
        sys_draw_rectangle(color, x, y, 1, 1);
    }
    
    result.end_ticks = benchmark_get_ticks();
    uint64_t end = benchmark_get_time_ms();
    
    result.total_time_ms = end - start;
    result.time_per_iteration = (double)result.total_time_ms / (double)pixel_count;
    result.operations_per_sec = (double)pixel_count / ((double)result.total_time_ms / 1000.0);
    
    return result;
}

BenchmarkResult benchmark_fps(uint64_t frame_count) {
    BenchmarkResult result;
    result.type = BENCH_FPS;
    result.name = "FPS Simulation (1000 ops/frame)";
    result.iterations = frame_count;
    
    // Benchmark real
    uint64_t start = benchmark_get_time_ms();
    result.start_ticks = benchmark_get_ticks();
    
    volatile uint64_t work = 0;
    for (uint64_t frame = 0; frame < frame_count; frame++) {
        // Simular trabajo de un frame (cálculos, lógica, etc.)
        for (int i = 0; i < 1000; i++) {
            work = (work + frame * i) ^ (i << 3);
        }
    }
    
    result.end_ticks = benchmark_get_ticks();
    uint64_t end = benchmark_get_time_ms();
    
    result.total_time_ms = end - start;
    result.time_per_iteration = (double)result.total_time_ms / (double)frame_count;
    
    // Calcular FPS
    double total_seconds = (double)result.total_time_ms / 1000.0;
    result.operations_per_sec = (double)frame_count / total_seconds; // FPS
    
    // Prevenir optimización
    if (work > 0) {
        result.operations_per_sec += 0;
    }
    
    return result;
}

BenchmarkResult benchmark_custom(const char *name, void (*func)(void), uint64_t iterations) {
    BenchmarkResult result;
    result.type = BENCH_CUSTOM;
    result.name = name;
    result.iterations = iterations;
    
    // Warmup
    for (uint64_t i = 0; i < BENCHMARK_WARMUP_ITERATIONS; i++) {
        func();
    }
    
    // Benchmark real
    uint64_t start = benchmark_get_time_ms();
    result.start_ticks = benchmark_get_ticks();
    
    for (uint64_t i = 0; i < iterations; i++) {
        func();
    }
    
    result.end_ticks = benchmark_get_ticks();
    uint64_t end = benchmark_get_time_ms();
    
    result.total_time_ms = end - start;
    result.time_per_iteration = (double)result.total_time_ms / (double)iterations;
    result.operations_per_sec = (double)iterations / ((double)result.total_time_ms / 1000.0);
    
    return result;
}

// ============== IMPRESIÓN DE RESULTADOS ==============

void benchmark_print_result(BenchmarkResult *result) {
    puts("\n", 0xFFFFFF);
    puts("========================================\n", 0x00FFFF);
    printf("Benchmark: %s\n", result->name);
    puts("========================================\n", 0x00FFFF);
    printf("Iteraciones:     %llu\n", result->iterations);
    printf("Tiempo total:    %llu ms\n", result->total_time_ms);
    printf("Tiempo/iter:     %.6f ms\n", result->time_per_iteration);
    
    // Mostrar métrica apropiada según el tipo
    switch (result->type) {
        case BENCH_FPS:
            printf("FPS:             %.2f\n", result->operations_per_sec);
            break;
        case BENCH_MEMORY_ACCESS:
            printf("Throughput:      %.2f MB/s\n", result->operations_per_sec);
            break;
        case BENCH_DRAW_PIXELS:
            printf("Pixels/sec:      %.0f\n", result->operations_per_sec);
            break;
        default:
            printf("Ops/sec:         %.0f\n", result->operations_per_sec);
            break;
    }
    
    printf("Ticks (inicio):  %llu\n", result->start_ticks);
    printf("Ticks (fin):     %llu\n", result->end_ticks);
    puts("========================================\n", 0x00FFFF);
}

void benchmark_print_suite(BenchmarkSuite *suite) {
    puts("\n\n", 0xFFFFFF);
    puts("########################################\n", 0xFFD700);
    printf("  BENCHMARK SUITE: %s\n", suite->suite_name);
    puts("########################################\n", 0xFFD700);
    printf("Tests ejecutados: %d\n", suite->test_count);
    printf("Tiempo total:     %llu ms\n", suite->total_time_ms);
    puts("########################################\n", 0xFFD700);
    
    for (int i = 0; i < suite->test_count; i++) {
        benchmark_print_result(&suite->results[i]);
    }
    
    puts("\n=== Suite completada ===\n\n", 0x00FF00);
}

// ============== SUITE COMPLETA ==============

void benchmark_run_full_suite(void) {
    puts("\n\n", 0xFFFFFF);
    puts("╔════════════════════════════════════════╗\n", 0xFFD700);
    puts("║   BENCHMARK SUITE - Sistema Completo  ║\n", 0xFFD700);
    puts("╚════════════════════════════════════════╝\n", 0xFFD700);
    puts("\n", 0xFFFFFF);
    
    // Array estático para resultados
    #define MAX_TESTS 10
    static BenchmarkResult results[MAX_TESTS];
    
    BenchmarkSuite suite;
    suite.suite_name = "Full System Benchmark";
    suite.test_count = 0;
    suite.results = results;
    suite.total_time_ms = 0;
    
    uint64_t suite_start = benchmark_get_time_ms();
    
    // Test 1: Operaciones enteras
    puts("[1/6] Ejecutando: Integer Operations...\n", 0x00FFFF);
    results[suite.test_count++] = benchmark_int_operations(100000);
    
    // Test 2: Operaciones de punto flotante
    puts("[2/6] Ejecutando: Float Operations...\n", 0x00FFFF);
    results[suite.test_count++] = benchmark_float_operations(50000);
    
    // Test 3: Acceso a memoria
    puts("[3/6] Ejecutando: Memory Access...\n", 0x00FFFF);
    results[suite.test_count++] = benchmark_memory_access(4, 1000);
    
    // Test 4: Overhead de syscalls
    puts("[4/6] Ejecutando: Syscall Overhead...\n", 0x00FFFF);
    results[suite.test_count++] = benchmark_syscall_overhead(10000);
    
    // Test 5: FPS
    puts("[5/6] Ejecutando: FPS Simulation...\n", 0x00FFFF);
    results[suite.test_count++] = benchmark_fps(1000);
    
    // Test 6: Dibujado de píxeles
    puts("[6/6] Ejecutando: Draw Pixels...\n", 0x00FFFF);
    results[suite.test_count++] = benchmark_draw_pixels(1000);
    
    uint64_t suite_end = benchmark_get_time_ms();
    suite.total_time_ms = suite_end - suite_start;
    
    // Imprimir resultados
    benchmark_print_suite(&suite);
}
