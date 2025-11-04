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

static char input_buffer[MAX_COMMAND_LENGTH];

static void print_prompt(void);

void shell_main(void) {
    sys_clear_screen();
    
    puts("  ==============================================================\n", COLOR_MAGENTA);
    puts("    Sistema Operativo de 64 bits | Arquitectura x86-64\n", COLOR_WHITE);
    puts("    Desarrollado por Grupo 12 | ITBA 2025\n", COLOR_YELLOW);
    puts("  ==============================================================\n", COLOR_MAGENTA);
    puts("\n  Escriba 'help' para ver comandos disponibles\n", COLOR_GREEN);

    int isExit = 0;
    while (!isExit) {
        print_prompt();
        int len = read_line(input_buffer, MAX_COMMAND_LENGTH);
        if (len > 0) {
            parse_and_execute(input_buffer, &isExit);
        }
    }
}

static void print_prompt(void) {
    puts("\n$ ", PROMPT_COLOR);
}