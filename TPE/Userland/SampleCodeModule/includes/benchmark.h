#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdint.h>

// ============== CONFIGURACIÓN ==============
#define MAX_BENCHMARK_ITERATIONS 1000000
#define BENCHMARK_WARMUP_ITERATIONS 100

// ============== TIPOS DE BENCHMARK ==============
typedef enum {
    BENCH_FPS,              // Frames por segundo (simulado con loops)
    BENCH_FLOAT_OPS,        // Operaciones de punto flotante
    BENCH_INT_OPS,          // Operaciones enteras
    BENCH_MEMORY_ACCESS,    // Acceso a memoria
    BENCH_SYSCALL_OVERHEAD, // Overhead de syscalls
    BENCH_DRAW_PIXELS,      // Dibujado de píxeles
    BENCH_CUSTOM            // Benchmark personalizado
} BenchmarkType;

// ============== ESTRUCTURAS ==============

// Resultado de un benchmark individual
typedef struct {
    BenchmarkType type;
    const char *name;
    uint64_t iterations;        // Número de iteraciones ejecutadas
    uint64_t total_time_ms;     // Tiempo total en milisegundos
    double time_per_iteration;  // Tiempo promedio por iteración (ms)
    double operations_per_sec;  // Operaciones por segundo
    uint64_t start_ticks;       // Tick inicial (para debugging)
    uint64_t end_ticks;         // Tick final (para debugging)
} BenchmarkResult;

// Contexto de benchmarking
typedef struct {
    const char *suite_name;     // Nombre del conjunto de tests
    int test_count;             // Número de tests ejecutados
    BenchmarkResult *results;   // Array de resultados
    uint64_t total_time_ms;     // Tiempo total de todos los tests
} BenchmarkSuite;

// ============== FUNCIONES PRINCIPALES ==============

/**
 * @brief Inicializa un suite de benchmarks
 * @param suite Puntero al suite a inicializar
 * @param name Nombre del suite
 * @param max_tests Número máximo de tests que contendrá
 */
void benchmark_init_suite(BenchmarkSuite *suite, const char *name, int max_tests);

/**
 * @brief Libera recursos de un suite
 * @param suite Puntero al suite
 */
void benchmark_free_suite(BenchmarkSuite *suite);

/**
 * @brief Ejecuta un benchmark de operaciones de punto flotante
 * @param iterations Número de iteraciones
 * @return Resultado del benchmark
 */
BenchmarkResult benchmark_float_operations(uint64_t iterations);

/**
 * @brief Ejecuta un benchmark de operaciones enteras
 * @param iterations Número de iteraciones
 * @return Resultado del benchmark
 */
BenchmarkResult benchmark_int_operations(uint64_t iterations);

/**
 * @brief Ejecuta un benchmark de acceso a memoria
 * @param size_kb Tamaño del buffer en KB
 * @param iterations Número de iteraciones
 * @return Resultado del benchmark
 */
BenchmarkResult benchmark_memory_access(uint64_t size_kb, uint64_t iterations);

/**
 * @brief Ejecuta un benchmark de overhead de syscalls
 * @param iterations Número de syscalls a ejecutar
 * @return Resultado del benchmark
 */
BenchmarkResult benchmark_syscall_overhead(uint64_t iterations);

/**
 * @brief Ejecuta un benchmark de dibujado de píxeles
 * @param pixel_count Número de píxeles a dibujar
 * @return Resultado del benchmark
 */
BenchmarkResult benchmark_draw_pixels(uint64_t pixel_count);

/**
 * @brief Ejecuta un benchmark de FPS simulado
 * @param frame_count Número de frames a simular
 * @return Resultado del benchmark
 */
BenchmarkResult benchmark_fps(uint64_t frame_count);

/**
 * @brief Mide el tiempo de ejecución de una función personalizada
 * @param name Nombre del benchmark
 * @param func Función a ejecutar
 * @param iterations Número de iteraciones
 * @return Resultado del benchmark
 */
BenchmarkResult benchmark_custom(const char *name, void (*func)(void), uint64_t iterations);

// ============== FUNCIONES DE UTILIDAD ==============

/**
 * @brief Obtiene el tiempo actual en milisegundos
 * @return Tiempo en ms
 */
uint64_t benchmark_get_time_ms(void);

/**
 * @brief Obtiene el número de ticks actuales
 * @return Número de ticks
 */
uint64_t benchmark_get_ticks(void);

/**
 * @brief Imprime el resultado de un benchmark
 * @param result Resultado a imprimir
 */
void benchmark_print_result(BenchmarkResult *result);

/**
 * @brief Imprime resumen de un suite completo
 * @param suite Suite a imprimir
 */
void benchmark_print_suite(BenchmarkSuite *suite);

/**
 * @brief Ejecuta una suite completa de benchmarks estándar
 * Ejecuta todos los benchmarks predefinidos y muestra resultados
 */
void benchmark_run_full_suite(void);

// ============== MACROS ÚTILES ==============

// Macro para medir tiempo de un bloque de código
#define BENCHMARK_START() uint64_t __bench_start = benchmark_get_time_ms()
#define BENCHMARK_END(name) do { \
    uint64_t __bench_end = benchmark_get_time_ms(); \
    uint64_t __bench_elapsed = __bench_end - __bench_start; \
    printf("%s: %llu ms\n", name, __bench_elapsed); \
} while(0)

#endif // BENCHMARK_H
