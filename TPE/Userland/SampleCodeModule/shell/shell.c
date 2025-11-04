/**
 * @file shell.c
 * @brief Shell interactiva del sistema operativo QuantumOS
 * @date 2025
 * @author Grupo 12 - ITBA
 */

#include "syscalls.h"
#include "io_utils.h"
#include "string_utils.h"
#include "tron.h"
#include "shell.h"
#include "commands.h"
#include "tronLogo.h"
#include <stddef.h>

// ============== VARIABLES GLOBALES ==============

static char input_buffer[MAX_COMMAND_LENGTH];

// Prototipos de funciones auxiliares
static void print_prompt(void);
/**
 * @brief Punto de entrada de la shell
 */
void shell_main(void) {
    sys_clear_screen();
    
    // Banner de bienvenida
    puts("\n", COLOR_CYAN);
    puts("   ___                   _                   ___  ___\n", COLOR_CYAN);
    puts("  / _ \\ _   _  __ _ _ __ | |_ _   _ _ __ ___ / _ \\/ __|\n", COLOR_CYAN);
    puts(" | | | | | | |/ _` | '_ \\| __| | | | '_ ` _ \\ | | \\__ \\\n", COLOR_CYAN);
    puts(" | |_| | |_| | (_| | | | | |_| |_| | | | | | | |_| |__) |\n", COLOR_CYAN);
    puts("  \\__\\_\\\\__,_|\\__,_|_| |_|\\__|\\__,_|_| |_| |_|\\___/____/\n", COLOR_CYAN);
    puts("\n", COLOR_WHITE);
    puts("  ==============================================================\n", COLOR_MAGENTA);
    puts("    Sistema Operativo de 64 bits | Arquitectura x86-64\n", COLOR_WHITE);
    puts("    Desarrollado por Grupo 12 | ITBA 2025\n", COLOR_YELLOW);
    puts("  ==============================================================\n", COLOR_MAGENTA);
    puts("\n  Escriba 'help' para ver comandos disponibles\n", COLOR_GREEN);


    int isExit = 0;
    // Loop principal de la shell
    while (!isExit) {
        print_prompt();
        
        int len = read_line(input_buffer, MAX_COMMAND_LENGTH);
        if (len > 0) {
            parse_and_execute(input_buffer, &isExit);
        }
    }
}

/**
 * @brief Imprime el prompt de la shell
 */
static void print_prompt(void) {
    puts("\n$ ", PROMPT_COLOR);
}