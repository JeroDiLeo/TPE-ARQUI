/**
 * @file shell.c
 * @brief Shell interactiva del sistema operativo JerOS
 * @date 2025
 */

#include "includes/syscalls.h"
#include "includes/lib.h"
#include "includes/tron.h"

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

// Configuración
#define TAB_SIZE 4
#define ESC 27
#define DEFAULT_SCALE 1

// Mensaje de bienvenida
#define WELCOME_MSG "Bienvenido al MejorOS. Fecha: %02d/%02d/20%02d\n"

// ============== ESTRUCTURAS ==============

// Estructura de comando
typedef struct {
    const char *name;
    void (*handler)(int argc, char **argv);
    const char *description;
    const char *usage;
} Command;

// ============== FORWARD DECLARATIONS ==============

// Comandos
static void cmd_help(int argc, char **argv);
static void cmd_time(int argc, char **argv);
static void cmd_inforeg(int argc, char **argv);
static void cmd_clear(int argc, char **argv);
static void cmd_scale(int argc, char **argv);
static void cmd_divzero(int argc, char **argv);
static void cmd_invopcode(int argc, char **argv);
static void cmd_printmem(int argc, char **argv);
static void cmd_tron(int argc, char **argv);
static void cmd_beep(int argc, char **argv);

// Funciones auxiliares de shell
static void shell_loop(void);
static void print_prompt(void);
static void get_input_and_print(char *input);
static void print_cursor(uint32_t hexColor, uint64_t offsetX);
static int no_screen_space(void);
static void parse_and_execute(char *input);
static int tokenize(char *input, char **argv, int max_args);
static const Command *find_command(const char *name);

// ============== TABLA DE COMANDOS ==============

static const Command commands[MAXCMD] = {
    {
        .name = "help",
        .handler = cmd_help,
        .description = "Muestra lista de comandos disponibles",
        .usage = "help"
    },
    {
        .name = "time",
        .handler = cmd_time,
        .description = "Muestra fecha y hora actual",
        .usage = "time"
    },
    {
        .name = "inforeg",
        .handler = cmd_inforeg,
        .description = "Muestra registros capturados (presionar Ctrl+R primero)",
        .usage = "inforeg"
    },
    {
        .name = "clear",
        .handler = cmd_clear,
        .description = "Limpia la pantalla",
        .usage = "clear"
    },
    {
        .name = "scale",
        .handler = cmd_scale,
        .description = "Cambia escala de fuente (1-3)",
        .usage = "scale <1|2|3>"
    },
    {
        .name = "divzero",
        .handler = cmd_divzero,
        .description = "Prueba excepción de división por cero",
        .usage = "divzero"
    },
    {
        .name = "invopcode",
        .handler = cmd_invopcode,
        .description = "Prueba excepción de opcode inválido",
        .usage = "invopcode"
    },
    {
        .name = "printmem",
        .handler = cmd_printmem,
        .description = "Imprime 32 bytes desde dirección de memoria",
        .usage = "printmem <address>"
    },
    {
        .name = "tron",
        .handler = cmd_tron,
        .description = "Jugar Tron (WASD vs IJKL, ESC para salir)",
        .usage = "tron"
    },
    {
        .name = "beep",
        .handler = cmd_beep,
        .description = "Reproduce un sonido",
        .usage = "beep <frequency> <duration_ms>"
    },

};

// ============== VARIABLES GLOBALES ==============

static char input_buffer[MAX_COMMAND_LENGTH];
static char *argv_array[MAX_ARGS];
static char *username = "user";

// Prototipos de funciones auxiliares
static void print_prompt(void);
static void parse_and_execute(char *input);
static int tokenize(char *input, char **argv, int max_args);
static const Command *find_command(const char *name);

/**
 * @brief Punto de entrada de la shell
 */
int main(void) {
    sys_clear_screen();
    
    // Banner de bienvenida
    puts("╔════════════════════════════════════════╗\n", COLOR_CYAN);
    puts("║       Bienvenido a JerOS Shell        ║\n", COLOR_CYAN);
    puts("║       Arquitectura de Computadoras    ║\n", COLOR_CYAN);
    puts("╚════════════════════════════════════════╝\n", COLOR_CYAN);
    puts("\nEscriba 'help' para ver comandos disponibles\n", COLOR_WHITE);
    
    // Loop principal de la shell
    while (1) {
        print_prompt();
        
        int len = read_line(input_buffer, MAX_COMMAND_LENGTH);
        if (len > 0) {
            parse_and_execute(input_buffer);
        }
    }
    
    return 0;
}

/**
 * @brief Imprime el prompt de la shell
 */
static void print_prompt(void) {
    puts("\n$ ", PROMPT_COLOR);
}

/**
 * @brief Parsea y ejecuta un comando
 */
static void parse_and_execute(char *input) {
    // Tokenizar entrada
    int argc = tokenize(input, argv_array, MAX_ARGS);
    
    if (argc == 0) {
        return;  // Línea vacía
    }
    
    // Buscar comando
    const Command *cmd = find_command(argv_array[0]);
    
    if (cmd == NULL) {
        puts("Error: Comando no encontrado '", ERROR_COLOR);
        puts(argv_array[0], ERROR_COLOR);
        puts("'\n", ERROR_COLOR);
        puts("Escriba 'help' para ver comandos disponibles\n", INFO_COLOR);
        return;
    }
    
    // Ejecutar handler
    cmd->handler(argc, argv_array);
}

/**
 * @brief Divide una cadena en tokens (palabras separadas por espacios)
 * @return Cantidad de tokens encontrados
 */
static int tokenize(char *input, char **argv, int max_args) {
    int argc = 0;
    int in_token = 0;
    
    for (char *p = input; *p != '\0' && argc < max_args; p++) {
        if (*p == ' ' || *p == '\t' || *p == '\n') {
            *p = '\0';
            in_token = 0;
        } else if (!in_token) {
            argv[argc++] = p;
            in_token = 1;
        }
    }
    
    return argc;
}

/**
 * @brief Busca un comando en la tabla de comandos
 * @return Puntero al comando o NULL si no se encuentra
 */
static const Command *find_command(const char *name) {
    for (int i = 0; i < MAXCMD; i++) {
        if (commands[i].name != NULL && strcmp(name, commands[i].name) == 0) {
            return &commands[i];
        }
    }
    return NULL;
}