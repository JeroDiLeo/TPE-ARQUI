/**
 * @file io_utils.c
 * @brief Funciones de I/O y memoria
 */

#include "includes/lib.h"
#include "includes/syscalls.h"

// File descriptors
#define STDIN  0
#define STDOUT 1
#define STDERR 2

// ============== I/O HELPERS ==============

/**
 * @brief Imprime un carácter con color
 */
void putchar(char c, uint64_t color) {
    sys_write(STDOUT, &c, 1, color);
}

/**
 * @brief Imprime una cadena con color
 */
void puts(const char *str, uint64_t color) {
    // Necesitamos strlen para calcular longitud
    int len = 0;
    const char *s = str;
    while (*s != '\0') {
        len++;
        s++;
    }
    sys_write(STDOUT, str, len, color);
}

/**
 * @brief Lee un carácter del teclado
 * @return Carácter leído o -1 si no hay entrada
 */
int getchar(void) {
    char c;
    int bytes_read = sys_read(STDIN, &c, 1);
    if (bytes_read > 0) {
        return c;
    }
    return -1;
}

/**
 * @brief Lee una línea completa con echo
 * @return Cantidad de caracteres leídos (sin contar \0)
 */
int read_line(char *buffer, int max_length) {
    int i = 0;
    char c;
    
    while (i < max_length - 1) {
        int bytes = sys_read(STDIN, &c, 1);
        
        if (bytes <= 0) {
            continue;  // No hay entrada disponible
        }
        
        // Enter: terminar línea
        if (c == '\n') {
            putchar('\n', COLOR_WHITE);
            break;
        }
        
        // Backspace: borrar carácter
        if (c == '\b') {
            if (i > 0) {
                i--;
                putchar('\b', COLOR_WHITE);  // El driver de video maneja el backspace
            }
            continue;
        }
        
        // Carácter imprimible: agregar y hacer echo
        if (c >= 32 && c <= 126) {
            buffer[i++] = c;
            putchar(c, COLOR_WHITE);
        }
    }
    
    buffer[i] = '\0';
    return i;
}
