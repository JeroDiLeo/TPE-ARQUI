/**
 * @file shell.c
 * @brief Shell interactiva del sistema operativo JerOS
 * @date 2025
 */

#include "syscalls.h"
#include "io_utils.h"
#include "string_utils.h"
#include "tron.h"
#include "shell.h"
#include "commands.h"
// ============== CONSTANTES ==============

#define MAX_COMMAND_LENGTH 256
#define MAX_ARGS 10
#define NULL ((void*)0)
#define MAXCMD 10

// File descriptors
#define STDIN  0
#define STDOUT 1
#define STDERR 2

// Colores de la shell
#define USER_COLOR       0x0000AFFF
#define PROMPT_COLOR     0x005FD700
#define CURSOR_COLOR     COLOR_CYAN
#define SCREEN_COLOR     COLOR_BLACK
#define WHITE            COLOR_WHITE
#define ERROR_COLOR      0xFF0000
#define INFO_COLOR       0x00FFFF

// Configuracion
#define TAB_SIZE 4
#define ESC 27
#define DEFAULT_SCALE 1

// Mensaje de bienvenida
#define WELCOME_MSG "Bienvenido al MejorOS. Fecha: %02d/%02d/20%02d\n"

// ============== ESTRUCTURAS ==============

//===





// ============== VARIABLES GLOBALES ==============

static char input_buffer[MAX_COMMAND_LENGTH];

// Prototipos de funciones auxiliares
static void print_prompt(void);
static void print_prompt(void) {
    const char *user = "user";
    const char *host = "JerOS";

    sys_write(STDOUT, user, strlen(user), USER_COLOR);
    sys_write(STDOUT, "@", 1, PROMPT_COLOR);
    sys_write(STDOUT, host, strlen(host), PROMPT_COLOR);
    sys_write(STDOUT, ":$ ", 3, PROMPT_COLOR);
}
//Esto del main va en la parte de loader
/**
 * @brief Punto de entrada de la shell
 */
int shell_main(void) {
    sys_clear_screen();
    
    // Banner de bienvenida
    puts("+========================================+\n", COLOR_CYAN);
    puts("|        Bienvenido a JerOS Shell        |\n", COLOR_CYAN);
    puts("|     Arquitectura de Computadoras       |\n", COLOR_CYAN);
    puts("+========================================+\n", COLOR_CYAN);
    puts("\nEscriba 'help' para ver comandos disponibles\n", COLOR_WHITE);
    
    // Loop principal de la shell
    // while (1) {
    //     print_prompt();
        
    //     int len = read_line(input_buffer, MAX_COMMAND_LENGTH);
    //     if (len > 0) {
    //         parse_and_execute(input_buffer);
    //     }
    // }
    
    return 0;
}
