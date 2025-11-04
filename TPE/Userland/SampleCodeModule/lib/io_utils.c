/**
 * @file io_utils.c
 * @brief Funciones de I/O y memoria
 */

#include "io_utils.h"
#include "syscalls.h"
#include "string_utils.h"
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
// Helper para imprimir con padding
static void print_with_padding(const char *str, int width, char pad_char, int *printed_chars) {
    int len = strlen(str);
    
    // Agregar padding a la izquierda
    for (int i = len; i < width; i++) {
        putcharColor(pad_char, COLOR_WHITE);
        (*printed_chars)++;
    }
    
    puts(str, COLOR_WHITE);
    (*printed_chars) += len;
}

// Helper para parsear el ancho del formato (ej: "02" de "%02d")
static int parse_format_width(const char *format, int *index, int *zero_pad) {
    int width = 0;
    *zero_pad = (format[*index] == '0');
    
    while (format[*index] >= '0' && format[*index] <= '9') {
        width = width * 10 + (format[*index] - '0');
        (*index)++;
    }
    
    return width;
}

int printf(const char *format, ...) {
    va_list arguments;
    va_start(arguments, format);
    
    char conversion_buffer[CONVERSION_BUFFER_SIZE];
    int printed_chars = 0;
    int i = 0;
    
    while (format[i] != '\0') {
        if (format[i] == '%' && format[i + 1] != '\0') {
            i++; // Saltar el '%'
            
            // Verificar si hay especificador de ancho (ej: 02, 04)
            int width = 0;
            int zero_pad = 0;
            
            if (format[i] >= '0' && format[i] <= '9') {
                width = parse_format_width(format, &i, &zero_pad);
            }
            
            // Procesar el tipo de formato
            // Verificar modificadores 'l' y 'll'
            int long_mod = 0;  // 1 para 'l', 2 para 'll'
            
            if (format[i] == 'l') {
                long_mod = 1;
                i++;
                if (format[i] == 'l') {
                    long_mod = 2;
                    i++;
                }
            }
            
            switch (format[i]) {
                case 's': {
                    const char *string_arg = va_arg(arguments, const char *);
                    puts(string_arg, COLOR_WHITE);
                    printed_chars += strlen(string_arg);
                    break;
                }
                
                case 'd': {
                    int decimal_number = va_arg(arguments, int);
                    itoa(decimal_number, conversion_buffer, 10);
                    
                    if (width > 0) {
                        char pad_char = zero_pad ? '0' : ' ';
                        print_with_padding(conversion_buffer, width, pad_char, &printed_chars);
                    } else {
                        puts(conversion_buffer, COLOR_WHITE);
                        printed_chars += strlen(conversion_buffer);
                    }
                    break;
                }
                
                case 'u': {
                    uint64_t unsigned_number;
                    
                    if (long_mod == 2) {
                        // %llu - unsigned long long
                        unsigned_number = va_arg(arguments, uint64_t);
                    } else if (long_mod == 1) {
                        // %lu - unsigned long
                        unsigned_number = va_arg(arguments, unsigned long);
                    } else {
                        // %u - unsigned int
                        unsigned_number = va_arg(arguments, unsigned int);
                    }
                    
                    // Convertir a string (base 10)
                    itoa_hex(unsigned_number, conversion_buffer);
                    // Necesitamos itoa en base 10 para unsigned, usaremos itoa_hex temporalmente
                    // Mejor: hacer conversión manual
                    if (unsigned_number == 0) {
                        conversion_buffer[0] = '0';
                        conversion_buffer[1] = '\0';
                    } else {
                        char temp[32];
                        int idx = 0;
                        uint64_t n = unsigned_number;
                        
                        while (n > 0) {
                            temp[idx++] = '0' + (n % 10);
                            n /= 10;
                        }
                        
                        // Invertir
                        for (int j = 0; j < idx; j++) {
                            conversion_buffer[j] = temp[idx - 1 - j];
                        }
                        conversion_buffer[idx] = '\0';
                    }
                    
                    if (width > 0) {
                        char pad_char = zero_pad ? '0' : ' ';
                        print_with_padding(conversion_buffer, width, pad_char, &printed_chars);
                    } else {
                        puts(conversion_buffer, COLOR_WHITE);
                        printed_chars += strlen(conversion_buffer);
                    }
                    break;
                }
                
                case 'x': {
                    uint64_t hex_number = va_arg(arguments, uint64_t);
                    itoa_hex(hex_number, conversion_buffer);
                    
                    if (width > 0) {
                        char pad_char = zero_pad ? '0' : ' ';
                        print_with_padding(conversion_buffer, width, pad_char, &printed_chars);
                    } else {
                        puts(conversion_buffer, COLOR_WHITE);
                        printed_chars += strlen(conversion_buffer);
                    }
                    break;
                }
                
                case 'c': {
                    char character = (char)va_arg(arguments, int);
                    putcharColor(character, COLOR_WHITE);
                    printed_chars++;
                    break;
                }
                
                case '%': {
                    putcharColor('%', COLOR_WHITE);
                    printed_chars++;
                    break;
                }
                
                default: {
                    putcharColor('%', COLOR_WHITE);
                    putcharColor(format[i], COLOR_WHITE);
                    printed_chars += 2;
                    break;
                }
            }
        } else {
            putcharColor(format[i], COLOR_WHITE);
            printed_chars++;
        }
        
        i++;
    }
    
    va_end(arguments);
    return printed_chars;
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

// ============== FORMATTED INPUT ==============

/**
 * @brief Lee entrada formateada (scanf)
 * @param format Cadena de formato con especificadores %d, %s, %c, %x
 * @return Cantidad de elementos leídos exitosamente
 * 
 * Especificadores soportados:
 * - %d: Entero decimal
 * - %x: Entero hexadecimal (con o sin 0x)
 * - %s: String (hasta espacio/newline)
 * - %c: Un solo carácter
 */
int scanf(const char *format, ...) {
    va_list arguments;
    va_start(arguments, format);
    
    char input_buffer[256];
    int input_index = 0;
    int items_read = 0;
    int format_index = 0;
    
    // Leer toda la línea de entrada
    int line_length = read_line(input_buffer, sizeof(input_buffer));
    if (line_length == 0) {
        va_end(arguments);
        return 0;
    }
    
    // Procesar formato
    while (format[format_index] != '\0' && input_index < line_length) {
        // Saltar espacios en blanco en el formato
        if (format[format_index] == ' ' || format[format_index] == '\t') {
            format_index++;
            continue;
        }
        
        // Saltar espacios en blanco en la entrada
        while (input_index < line_length && 
               (input_buffer[input_index] == ' ' || input_buffer[input_index] == '\t')) {
            input_index++;
        }
        
        // Si encontramos un especificador de formato
        if (format[format_index] == '%' && format[format_index + 1] != '\0') {
            format_index++; // Saltar el '%'
            
            switch (format[format_index]) {
                case 'd': { // Entero decimal
                    int *int_ptr = va_arg(arguments, int *);
                    int sign = 1;
                    int value = 0;
                    int has_digits = 0;
                    
                    // Manejar signo
                    if (input_buffer[input_index] == '-') {
                        sign = -1;
                        input_index++;
                    } else if (input_buffer[input_index] == '+') {
                        input_index++;
                    }
                    
                    // Leer dígitos
                    while (input_index < line_length && isDigit(input_buffer[input_index])) {
                        value = value * 10 + (input_buffer[input_index] - '0');
                        input_index++;
                        has_digits = 1;
                    }
                    
                    if (has_digits) {
                        *int_ptr = sign * value;
                        items_read++;
                    } else {
                        va_end(arguments);
                        return items_read;
                    }
                    break;
                }
                
                case 'x': { // Entero hexadecimal
                    uint64_t *hex_ptr = va_arg(arguments, uint64_t *);
                    uint64_t value = 0;
                    int has_digits = 0;
                    
                    // Saltar prefijo "0x" si existe
                    if (input_index < line_length - 1 && 
                        input_buffer[input_index] == '0' && 
                        (input_buffer[input_index + 1] == 'x' || input_buffer[input_index + 1] == 'X')) {
                        input_index += 2;
                    }
                    
                    // Leer dígitos hexadecimales
                    while (input_index < line_length) {
                        char c = input_buffer[input_index];
                        if (c >= '0' && c <= '9') {
                            value = value * 16 + (c - '0');
                            has_digits = 1;
                        } else if (c >= 'a' && c <= 'f') {
                            value = value * 16 + (c - 'a' + 10);
                            has_digits = 1;
                        } else if (c >= 'A' && c <= 'F') {
                            value = value * 16 + (c - 'A' + 10);
                            has_digits = 1;
                        } else {
                            break;
                        }
                        input_index++;
                    }
                    
                    if (has_digits) {
                        *hex_ptr = value;
                        items_read++;
                    } else {
                        va_end(arguments);
                        return items_read;
                    }
                    break;
                }
                
                case 's': { // String (hasta espacio o fin)
                    char *str_ptr = va_arg(arguments, char *);
                    int str_index = 0;
                    
                    // Leer hasta espacio, tab o fin
                    while (input_index < line_length && 
                           input_buffer[input_index] != ' ' && 
                           input_buffer[input_index] != '\t' &&
                           input_buffer[input_index] != '\0') {
                        str_ptr[str_index++] = input_buffer[input_index++];
                    }
                    
                    if (str_index > 0) {
                        str_ptr[str_index] = '\0';
                        items_read++;
                    } else {
                        va_end(arguments);
                        return items_read;
                    }
                    break;
                }
                
                case 'c': { // Un solo carácter
                    char *char_ptr = va_arg(arguments, char *);
                    
                    if (input_index < line_length) {
                        *char_ptr = input_buffer[input_index++];
                        items_read++;
                    } else {
                        va_end(arguments);
                        return items_read;
                    }
                    break;
                }
                
                default:
                    // Especificador no reconocido
                    format_index++;
                    continue;
            }
            
            format_index++;
        } else {
            // Carácter literal en el formato (debe coincidir con la entrada)
            if (format[format_index] == input_buffer[input_index]) {
                format_index++;
                input_index++;
            } else {
                // No coincide, terminar
                break;
            }
        }
    }
    
    va_end(arguments);
    return items_read;
}




