#ifndef STRING_UTILS_H
#define STRING_UTILS_H

// ============== STRING FUNCTIONS ==============

/**
 * @brief Calcula la longitud de una cadena
 * @param str Cadena a medir
 * @return Cantidad de caracteres (sin contar '\0')
 */
int strlen(const char *str);

/**
 * @brief Compara dos cadenas lexicográficamente
 * @param s1 Primera cadena
 * @param s2 Segunda cadena
 * @return 0 si son iguales, <0 si s1 < s2, >0 si s1 > s2
 */
int strcmp(const char *s1, const char *s2);

/**
 * @brief Compara los primeros n caracteres de dos cadenas
 * @param s1 Primera cadena
 * @param s2 Segunda cadena
 * @param n Cantidad de caracteres a comparar
 * @return 0 si son iguales, <0 si s1 < s2, >0 si s1 > s2
 */
int strncmp(const char *s1, const char *s2, int n);

/**
 * @brief Copia una cadena en otra
 * @param dest Cadena destino
 * @param src Cadena origen
 * @return Puntero a dest
 */
char *strcpy(char *dest, const char *src);

/**
 * @brief Concatena una cadena al final de otra
 * @param dest Cadena destino
 * @param src Cadena a concatenar
 * @return Puntero a dest
 */
char *strcat(char *dest, const char *src);

#endif
