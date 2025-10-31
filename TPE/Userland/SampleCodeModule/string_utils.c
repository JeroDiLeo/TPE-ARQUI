/**
 * @file string_utils.c
 * @brief Funciones de manipulación de strings y conversión
 */

#include "includes/lib.h"

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
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

/**
 * @brief Compara n caracteres de dos cadenas
 */
int strncmp(const char *s1, const char *s2, int n) {
    while (n > 0 && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) {
        return 0;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
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

// ============== CONVERSION FUNCTIONS ==============

/**
 * @brief Convierte string a entero (decimal)
 */
int atoi(const char *str) {
    int result = 0;
    int sign = 1;
    
    // Saltar espacios
    while (*str == ' ' || *str == '\t') {
        str++;
    }
    
    // Manejar signo
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    // Convertir dígitos
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return sign * result;
}

/**
 * @brief Convierte string hexadecimal a entero
 */
uint64_t atoi_hex(const char *str) {
    uint64_t result = 0;
    
    // Saltar "0x" si existe
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str += 2;
    }
    
    while (*str) {
        char c = *str++;
        
        if (c >= '0' && c <= '9') {
            result = result * 16 + (c - '0');
        } else if (c >= 'a' && c <= 'f') {
            result = result * 16 + (c - 'a' + 10);
        } else if (c >= 'A' && c <= 'F') {
            result = result * 16 + (c - 'A' + 10);
        } else {
            break;  // Carácter no válido
        }
    }
    
    return result;
}

/**
 * @brief Convierte entero a string en base dada
 */
void itoa(int value, char *buffer, int base) {
    if (base < 2 || base > 36) {
        *buffer = '\0';
        return;
    }
    
    char *ptr = buffer;
    int tmp_value;
    int is_negative = 0;
    
    // Manejar negativos solo en base 10
    if (value < 0 && base == 10) {
        is_negative = 1;
        value = -value;
    }
    
    // Procesar dígitos en reversa
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[tmp_value - value * base];
    } while (value);
    
    // Agregar signo si es negativo
    if (is_negative) {
        *ptr++ = '-';
    }
    
    *ptr-- = '\0';
    
    // Invertir cadena
    char *start = buffer;
    while (start < ptr) {
        char tmp = *start;
        *start++ = *ptr;
        *ptr-- = tmp;
    }
}

/**
 * @brief Convierte entero sin signo a string
 */
void uitoa(uint64_t value, char *buffer, int base) {
    if (base < 2 || base > 36) {
        *buffer = '\0';
        return;
    }
    
    char *ptr = buffer;
    uint64_t tmp_value;
    
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[tmp_value - value * base];
    } while (value);
    
    *ptr-- = '\0';
    
    // Invertir cadena
    char *start = buffer;
    while (start < ptr) {
        char tmp = *start;
        *start++ = *ptr;
        *ptr-- = tmp;
    }
}

/**
 * @brief Convierte entero a string hexadecimal con 0x prefix
 */
void itoa_hex(uint64_t value, char *buffer) {
    buffer[0] = '0';
    buffer[1] = 'x';
    
    char *ptr = buffer + 2;
    int digit_count = 0;
    
    // Si es 0, escribir "0x0"
    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return;
    }
    
    // Extraer dígitos en reversa
    while (value > 0) {
        int digit = value & 0xF;
        *ptr++ = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        value >>= 4;
        digit_count++;
    }
    
    *ptr-- = '\0';
    
    // Invertir los dígitos (sin tocar "0x")
    char *start = buffer + 2;
    while (start < ptr) {
        char tmp = *start;
        *start++ = *ptr;
        *ptr-- = tmp;
    }
}

/**
 * @brief Implementación simple de sprintf (solo soporta %d, %x, %s, %c)
 */
void sprintf(char *buffer, const char *format, ...) {
    // Esta es una implementación básica, se puede expandir según necesidad
    // Por ahora, solo copia el formato
    strcpy(buffer, format);
}
