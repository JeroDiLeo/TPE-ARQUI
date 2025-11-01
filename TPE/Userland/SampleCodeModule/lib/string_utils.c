
#include "string_utils.h"

// ============== STRING FUNCTIONS ==============

/**
 * @brief Calcula longitud de una cadena
 */
int strlen(const char *str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/**
 * @brief Compara dos cadenas
 * @return 0 si son iguales, <0 si s1 < s2, >0 si s1 > s2
 */
int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        unsigned char c1 = (unsigned char)*s1++;
        unsigned char c2 = (unsigned char)*s2++;
        if (c1 != c2) {
            return (int)c1 - (int)c2;
        }
    }
    return (int)(unsigned char)*s1 - (int)(unsigned char)*s2;
}

/**
 * @brief Compara n caracteres de dos cadenas   ======VER SI LO USAMOS AL FINAL=======
 */
int strncmp(const char *s1, const char *s2, int n) {
    if (n <= 0) {
        return 0;
    }

    for (int i = 0; i < n; ++i) {
        unsigned char c1 = (unsigned char)s1[i];
        unsigned char c2 = (unsigned char)s2[i];

        if (c1 != c2) {
            return (int)c1 - (int)c2;
        }
        if (c1 == '\0') {  // también implica c2 == '\0'
            return 0;
        }
    }
    return 0;
}


/**
 * @brief Copia cadena src a dest
 */
char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++) != '\0');
    return dest;
}

/**
 * @brief Concatena src al final de dest
 */
char *strcat(char *dest, const char *src) {
    char *d = dest;
    while (*d != '\0') {
        d++;
    }
    while ((*d++ = *src++) != '\0');
    return dest;
}


