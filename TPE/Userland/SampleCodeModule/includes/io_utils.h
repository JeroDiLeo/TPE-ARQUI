#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <stdint.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define COLOR_BLACK   0x000000
#define COLOR_WHITE   0xFFFFFF
#define COLOR_RED     0xFF0000
#define COLOR_GREEN   0x00FF00
#define COLOR_BLUE    0x0000FF
#define COLOR_YELLOW  0xFFFF00
#define COLOR_CYAN    0x00FFFF
#define COLOR_MAGENTA 0xFF00FF
#define COLOR_GRAY    0x808080
#define COLOR_ORANGE  0xFFA500

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

int printf(const char *format, ...);

#endif
