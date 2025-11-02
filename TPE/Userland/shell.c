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
    int (*handler)(int argc, char **argv);
    const char *description;
    const char *usage;
} Command;

// ============== FORWARD DECLARATIONS ==============

// Comandos
static int cmd_help(int argc, char **argv);
static int cmd_time(int argc, char **argv);
static int cmd_inforeg(int argc, char **argv);
static int cmd_clear(int argc, char **argv);
static int cmd_scale(int argc, char **argv);
static int cmd_divzero(int argc, char **argv);
static int cmd_invopcode(int argc, char **argv);
static int cmd_exit(int argc, char **argv);
static int cmd_tron(int argc, char **argv);
static int cmd_beep(int argc, char **argv);

// Funciones auxiliares de shell
static void print_prompt(void);
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
        .name = "exit",
        .handler = cmd_exit,
        .description = "Sale de la shell",
        .usage = "exit"
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
//=====COMANDOS=======(Agregar un cmd de exit)
static int cmd_help(int argc, char **argv) {
    puts("Comandos disponibles:\n", INFO_COLOR);
    for (int i = 0; i < MAXCMD; i++) {
        if (commands[i].name != NULL) {
            printf(" - %s: %s\n", commands[i].name, commands[i].description);
        }
    }
    return 0;
}

static int cmd_time(int argc, char **argv) {    //(Hacer getter para el time)
    uint64_t day = sys_get_time(0);
    uint64_t month = sys_get_time(1);
    uint64_t year = sys_get_time(2);
    uint64_t hours = sys_get_time(3);
    uint64_t minutes = sys_get_time(4);
    uint64_t seconds = sys_get_time(5);
    
    printf("Fecha: %02d/%02d/20%02d\n", day, month, year);
    printf("Hora: %02d:%02d:%02d\n", hours, minutes, seconds);
    return 0;
}

static int cmd_inforeg(int argc, char **argv) {
    #define TOTAL_REGS 17
    uint64_t registers[TOTAL_REGS];
    
    // Obtener registros capturados
    uint64_t result = sys_get_registers(registers);
    
    if (result == 0) {
        puts("No hay registros capturados. Presione Ctrl+R para capturar el estado actual.\n", ERROR_COLOR);
        return 0;
    }
    
    // Nombres de los registros (orden consistente con libasm.asm _updateReg)
    const char *register_names[TOTAL_REGS] = {
        "RAX", "RBX", "RCX", "RDX",
        "RSI", "RDI", "RBP", "R8 ",
        "R9 ", "R10", "R11", "R12",
        "R13", "R14", "R15", "RIP",
        "RSP"
    };
    
    puts("Estado de los registros (capturados con Ctrl+R):\n", INFO_COLOR);
    puts("================================================\n", INFO_COLOR);
    
    // Imprimir registros en formato de 2 columnas
    for (int i = 0; i < TOTAL_REGS; i++) {
        printf("%s: %x", register_names[i], registers[i]);
        
        // Dos columnas, salto de línea cada 2 registros
        if (i % 2 == 1) {
            printf("\n");
        } else {
            printf("    ");  // Espaciado entre columnas
        }
    }
    
    // Si el total es impar, agregar salto de línea final
    if (TOTAL_REGS % 2 == 1) {
        printf("\n");
    }
    
    return 0;  // Agregar return faltante
}

static int cmd_clear(int argc, char **argv) {
    sys_clear_screen();
    return 0;
}

static int cmd_scale(int argc, char **argv) {
    if (argc != 2) {
        puts("Uso: scale <1|2|3>\n", ERROR_COLOR);
        return -1;
    }
    
    int scale = atoi(argv[1]);
    if (scale < 1 || scale > 3) {
        puts("Error: Escala inválida. Debe ser 1, 2 o 3.\n", ERROR_COLOR);
        return -1;
    }
    
    sys_set_font_scale(scale);
    printf("Escala de fuente cambiada a %d.\n", scale);
    return 0;
}
//Estas dos son para REVISAR ademas desp del testearlas nose si son necesairas
static int cmd_divzero(int argc, char **argv) {
    puts("Provocando excepción de división por cero...\n", INFO_COLOR);
    volatile int a = 1;
    volatile int b = 0;
    volatile int c = a / b;  // Esto debería causar la excepción
    (void)c; // Evitar advertencia de variable no usada
    return 0;
}

static int cmd_invopcode(int argc, char **argv) {
    puts("Provocando excepción de opcode inválido...\n", INFO_COLOR);
    __asm__ volatile (
        ".byte 0x0F, 0x0B"  // Opcode inválido
    );
    return 0;
}

static int cmd_exit(int argc, char **argv) {
    puts("Saliendo de la shell...\n", INFO_COLOR);
    puts("Hasta luego!\n", COLOR_CYAN);
    // Loop infinito con halt para detener ejecución
    while(1) {
        __asm__ volatile ("hlt");
    }
    return 0;
}

static int cmd_tron(int argc, char **argv) {
    puts("Iniciando juego Tron...\n", INFO_COLOR);
    tron_game();
    // Limpiar pantalla al volver
    sys_clear_screen();
    puts("Juego terminado. Bienvenido de vuelta a la shell.\n", COLOR_CYAN);
    return 0;
}

static int cmd_beep(int argc, char **argv) {
    if (argc != 3) {
        puts("Uso: beep <frequency> <duration_ms>\n", ERROR_COLOR);
        puts("Ejemplo: beep 440 500  (reproduce La4 por 500ms)\n", INFO_COLOR);
        return -1;
    }
    
    int frequency = atoi(argv[1]);
    int duration = atoi(argv[2]);
    
    // Validar frecuencia (rango audible humano: 20Hz - 20000Hz)
    if (frequency < 20 || frequency > 20000) {
        puts("Error: Frecuencia inválida. Debe estar entre 20 y 20000 Hz.\n", ERROR_COLOR);
        return -1;
    }
    
    // Validar duración
    if (duration < 1 || duration > 10000) {
        puts("Error: Duración inválida. Debe estar entre 1 y 10000 ms.\n", ERROR_COLOR);
        return -1;
    }
    
    printf("Reproduciendo sonido: %d Hz por %d ms...\n", frequency, duration);
    sys_play_sound(frequency, duration);
    puts("Sonido reproducido!\n", COLOR_GREEN);
    
    return 0;
}
//===





// ============== VARIABLES GLOBALES ==============

static char input_buffer[MAX_COMMAND_LENGTH];
static char *argv_array[MAX_ARGS];

// Prototipos de funciones auxiliares
static void print_prompt(void);
static void parse_and_execute(char *input);
static int tokenize(char *input, char **argv, int max_args);
static const Command *find_command(const char *name);

//Esto del main va en la parte de loader
/**
 * @brief Punto de entrada de la shell
 */
int shell_main(void) {
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
    
    // Ejecutar handletokenizer
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