#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdint.h>

/**
 * @brief Ejecuta suite completa de benchmarks
 */
void run_benchmarks(void);

/**
 * @brief Ejecuta un benchmark específico
 * @param test_name Nombre del test (cpu, mem, sys, io)
 */
void run_specific_benchmark(const char *test_name);

/**
 * @brief Muestra información de ayuda sobre benchmarks
 */
void benchmark_help(void);

#endif
