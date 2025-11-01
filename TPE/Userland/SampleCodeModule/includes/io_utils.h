#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <stdint.h>

// ============== I/O FUNCTIONS ==============

/**
 * @brief Imprime un carácter con color
 * @param c Carácter a imprimir
 * @param color Color en formato RGB (0xRRGGBB)
 */
void putcharColor(char c, uint64_t color);

/**
 * @brief Imprime una cadena con color (sin salto de línea)
 * @param str Cadena a imprimir
 * @param color Color en formato RGB (0xRRGGBB)
 */
void puts(const char *str, uint64_t color);

/**
 * @brief Lee un carácter del teclado
 * @return Carácter leído o -1 si no hay entrada disponible
 */
int getchar(void);

/**
 * @brief Lee una línea completa con echo
 * @param buffer Buffer donde guardar la línea
 * @param max_length Tamaño máximo del buffer
 * @return Cantidad de caracteres leídos (sin contar '\0')
 */
int read_line(char *buffer, int max_length);

// ============== VALIDATION FUNCTIONS ==============

/**
 * @brief Verifica si un carácter es una letra
 * @param c Carácter a verificar
 * @return 1 si es letra (a-z, A-Z), 0 en caso contrario
 */
int isChar(char c);

/**
 * @brief Verifica si un carácter es un dígito
 * @param c Carácter a verificar
 * @return 1 si es dígito (0-9), 0 en caso contrario
 */
int isDigit(char c);

// ============== CONVERSION FUNCTIONS ==============

/**
 * @brief Convierte string a entero (decimal)
 * @param str Cadena a convertir
 * @return Valor entero
 */
int atoi(const char *str);

/**
 * @brief Convierte entero a string en base dada
 * @param value Valor a convertir
 * @param buffer Buffer donde guardar el resultado
 * @param base Base numérica (2-36)
 */
void itoa(int value, char *buffer, int base);

/**
 * @brief Convierte entero a string hexadecimal con prefijo "0x"
 * @param value Valor a convertir
 * @param buffer Buffer donde guardar el resultado (mínimo 19 bytes)
 */
void itoa_hex(uint64_t value, char *buffer);

// ============== FORMATTED OUTPUT ==============

/**
 * @brief Imprime texto formateado (como printf de C estándar)
 * @param format Cadena de formato con especificadores:
 *               %s - string
 *               %d - entero decimal
 *               %c - carácter
 *               %x - hexadecimal (con prefijo 0x)
 *               %% - símbolo '%' literal
 * @return Cantidad de caracteres impresos
 */
int printf(const char *format, ...);

#endif
