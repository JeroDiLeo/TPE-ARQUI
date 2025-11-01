/**
 * @file io_utils.c
 * @brief Funciones de I/O y memoria
 */

#include "io_utils.h"
#include "syscalls.h"
#include <stdarg.h>

// File descriptors
#define STDIN  0
#define STDOUT 1
#define STDERR 2

// Buffer size para conversiones en printf
#define CONVERSION_BUFFER_SIZE 64

// ============== I/O HELPERS ==============

/**
 * @brief Imprime un carácter con color
 */
void putcharColor(char c, uint64_t color) {
    sys_write(STDOUT, &c, 1, color);
}

/**
 * @brief Imprime una cadena con color
 */
void puts(const char *str, uint64_t color) {
    while (*str != '\0') {
        putcharColor(*str, color);
        str++;
    }
}

/**
 * @brief Lee un carácter del teclado
 * @return Carácter leído o -1 si no hay entrada
 */
int getchar() {
    char c;
    int bytes_read = sys_read(STDIN, &c, 1);
    if (bytes_read > 0) {
        return c;
    }
    return -1;
}

int isChar(char c){
    if((c>='a' && c<='z') || (c>='A' && c<='Z')){
        return 1;
    }
    return 0;
}

int isDigit(char c){
    if(c>='0' && c<='9'){
        return 1;
    }
    return 0;
}
//=================Funciones Importantes============================

//Printf  la vamos usar para imprimir los regs en pantalla, para toda la shell y tambien en el juego seguro

//imprime texto formateado y soporta strings enteros decimales y hexadecimales y caracteres

//puts imprime una cadena y agrega salto de linea al final
//Nos va a servir para en comandos mostrar la informacion, en la shell para los msg del so y para los msg del menu del juego
/**
 * @brief Imprime texto formateado con soporte para %s, %d, %c, %x
 * @param format Cadena de formato
 * @return Cantidad de caracteres impresos
 */
int printf(const char *format, ...) {
    // va_list arguments;
    // va_start(arguments, format);
    
    // char conversion_buffer[CONVERSION_BUFFER_SIZE];
    // int printed_chars = 0;
    // int format_index = 0;
    
    // while (format[format_index] != '\0') {
    //     // Si encontramos '%', procesar formato especial
    //     if (format[format_index] == '%' && format[format_index + 1] != '\0') {
    //         format_index++; // Avanzar al especificador
            
    //         switch (format[format_index]) {
    //             case 's': {
    //                 // Imprimir string
    //                 const char *string_arg = va_arg(arguments, const char *);
    //                 puts(string_arg, COLOR_WHITE);
    //                 printed_chars += strlen(string_arg);
    //                 break;
    //             }
                
    //             case 'd': {
    //                 // Imprimir entero decimal
    //                 int decimal_number = va_arg(arguments, int);
    //                 itoa(decimal_number, conversion_buffer, 10);
    //                 puts(conversion_buffer, COLOR_WHITE);
    //                 printed_chars += strlen(conversion_buffer);
    //                 break;
    //             }
                
    //             case 'c': {
    //                 // Imprimir carácter
    //                 char character = (char)va_arg(arguments, int);
    //                 putcharColor(character, COLOR_WHITE);
    //                 printed_chars++;
    //                 break;
    //             }
                
    //             case 'x': {
    //                 // Imprimir hexadecimal
    //                 uint64_t hex_number = va_arg(arguments, uint64_t);
    //                 itoa_hex(hex_number, conversion_buffer);
    //                 puts(conversion_buffer, COLOR_WHITE);
    //                 printed_chars += strlen(conversion_buffer);
    //                 break;
    //             }
                
    //             case '%': {
    //                 // Imprimir '%' literal
    //                 putcharColor('%', COLOR_WHITE);
    //                 printed_chars++;
    //                 break;
    //             }
                
    //             default: {
    //                 // Formato no reconocido, imprimir '%' y el carácter
    //                 putcharColor('%', COLOR_WHITE);
    //                 putcharColor(format[format_index], COLOR_WHITE);
    //                 printed_chars += 2;
    //                 break;
    //             }
    //         }
    //     } else {
    //         // Carácter normal, imprimir directamente
    //         putcharColor(format[format_index], COLOR_WHITE);
    //         printed_chars++;
    //     }
        
    //     format_index++;
    // }
    
    // va_end(arguments);
    // return printed_chars;
}
//===================================================================

//=================Funciones Graficas=============================

//DrawRectangle
//clearscreen

//================================================================

//=================Funciones de info de pantalla=================
//getScreenWidth y getScreenHeight (Dimensiones de la pantalla) Usado en la shell
//getCursorX y getCursorY (Obtener la pos actual del cursor) Usado en la shell para dibujarlo correctamente
//getFontWidth y getFontHeight (Dimensiones de los caracteres) Usado en la shell para el cursor
//setFontScale (Escala de los caracteres) Usado en la shell para agrandar el texto
//=================================================================




/**=======Verificar esta funcion=========
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
            putcharColor('\n', COLOR_WHITE);
            break;
        }
        
        // Backspace: borrar carácter
        if (c == '\b') {
            if (i > 0) {
                i--;
                putcharColor('\b', COLOR_WHITE);  // El driver de video maneja el backspace
            }
            continue;
        }
        
        // Carácter imprimible: agregar y hacer echo
        if (c >= 32 && c <= 126) {
            buffer[i++] = c;
            putcharColor(c, COLOR_WHITE);
        }
    }
    
    buffer[i] = '\0';
    return i;
}

// ============== CONVERSION FUNCTIONS ==============

/**
 * @brief Convierte string a entero (decimal)       (Chequeada)
 */
int atoi(const char *str) {
    int result = 0;
    int sign = 1;
    
    // Saltar espacios
    while (*str == ' ' || *str == '\t'){
        str++;
    }
    
    // Manejar signo
    if (*str == '-' || *str == '+'){
        sign = (*str == '-') ? -1 : 1;
        str++;
    }
    
    // Convertir dígitos
    while (isDigit(*str)) { 
        result = result * 10 + (*str - '0');
        str++;
    }

    return sign * result;
}




/**
 * @brief Convierte entero a string en base dada        (Se usa para el printf) (Revisarla)
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
 * @brief Convierte entero a string hexadecimal con 0x prefix
 */
void itoa_hex(uint64_t value, char *buffer) {
    buffer[0] = '0';
    buffer[1] = 'x';
    
    char *buffer_ptr = buffer + 2;
    
    // Si es 0, escribir "0x0"
    if (value == 0) {
        *buffer_ptr++ = '0';
        *buffer_ptr = '\0';
        return;
    }
    
    // Extraer dígitos en reversa
    while (value > 0) {
        int current_digit = value & 0xF;
        *buffer_ptr++ = (current_digit < 10) ? ('0' + current_digit) : ('A' + current_digit - 10);
        value >>= 4;
    }
    
    *buffer_ptr = '\0';
    
    // Invertir los dígitos (sin tocar "0x")
    char *start_ptr = buffer + 2;
    char *end_ptr = buffer_ptr - 1;
    while (start_ptr < end_ptr) {
        char temp_char = *start_ptr;
        *start_ptr++ = *end_ptr;
        *end_ptr-- = temp_char;
    }
}

/**
 * @brief Convierte string hexadecimal a entero      (Me PARECE QUE NO LA VAMOS A USAR)
 */
// uint64_t atoihex(const char *str) {
//     uint64_t result = 0;
    
//     // Saltar "0x" si existe
//     if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
//         str += 2;
//     }
    
//     while (*str) {
//         char c = *str++;
        
//         if (c >= '0' && c <= '9') {
//             result = result * 16 + (c - '0');
//         } else if (c >= 'a' && c <= 'f') {
//             result = result * 16 + (c - 'a' + 10);
//         } else if (c >= 'A' && c <= 'F') {
//             result = result * 16 + (c - 'A' + 10);
//         } else {
//             break;  // Carácter no válido
//         }
//     }
    
//     return result;
// }
/**
 * @brief Convierte entero sin signo a string           (No es necesaria )
 */
// void uitoa(uint64_t value, char *buffer, int base) {
//     if (base < 2 || base > 36) {
//         *buffer = '\0';
//         return;
//     }
    
//     char *ptr = buffer;
//     uint64_t tmp_value;
    
//     do {
//         tmp_value = value;
//         value /= base;
//         *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[tmp_value - value * base];
//     } while (value);
    
//     *ptr-- = '\0';
    
//     // Invertir cadena
//     char *start = buffer;
//     while (start < ptr) {
//         char tmp = *start;
//         *start++ = *ptr;
//         *ptr-- = tmp;
//     }
// }

// ============== FORMATTED OUTPUT ==============




