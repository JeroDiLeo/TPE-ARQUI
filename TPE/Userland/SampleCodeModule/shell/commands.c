/**
 * @file commands.c
 * @brief Implementación de comandos de la shell
 * @date 2025
 */

#include "syscalls.h"
#include "io_utils.h"
#include "string_utils.h"
#include "tron.h"
#include "shell.h"
#include "commands.h"
#include "time.h"
#include "exceptions.h"
#include "benchmark.h"
#include <stddef.h>

// ============== ESTRUCTURAS ==============

// Definición de la estructura Tcommand (el typedef está en commands.h)
struct Tcommand {
    const char *name;
    int (*handler)(int argc, char **argv);
    const char *description;
    const char *usage;
};

// ============== FORWARD DECLARATIONS ==============

// Comandos
static int cmd_help(int argc, char **argv);
static int cmd_time(int argc, char **argv);
static int cmd_timezone(int argc, char **argv);
static int cmd_inforeg(int argc, char **argv);
static int cmd_clear(int argc, char **argv);
static int cmd_scale(int argc, char **argv);
static int cmd_divzero(int argc, char **argv);
static int cmd_invopcode(int argc, char **argv);
static int cmd_exit(int argc, char **argv);
static int cmd_tron(int argc, char **argv);
static int cmd_beep(int argc, char **argv);
static int cmd_benchmark(int argc, char **argv);

// ============== TABLA DE COMANDOS ==============

static struct Tcommand cmd_help_s = {
    .name = "help",
    .handler = cmd_help,
    .description = "Muestra lista de comandos disponibles",
    .usage = "help"
};

static struct Tcommand cmd_time_s = {
    .name = "time",
    .handler = cmd_time,
    .description = "Muestra fecha y hora actual",
    .usage = "time"
};

static struct Tcommand cmd_timezone_s = {
    .name = "timezone",
    .handler = cmd_timezone,
    .description = "Configura el offset de zona horaria (ej: -3 para UTC-3)",
    .usage = "timezone <offset>"
};

static struct Tcommand cmd_inforeg_s = {
    .name = "inforeg",
    .handler = cmd_inforeg,
    .description = "Muestra registros capturados (presionar Ctrl+R primero)",
    .usage = "inforeg"
};

static struct Tcommand cmd_clear_s = {
    .name = "clear",
    .handler = cmd_clear,
    .description = "Limpia la pantalla",
    .usage = "clear"
};

static struct Tcommand cmd_scale_s = {
    .name = "scale",
    .handler = cmd_scale,
    .description = "Cambia escala de fuente (1-3)",
    .usage = "scale <1|2|3>"
};

static struct Tcommand cmd_divzero_s = {
    .name = "divzero",
    .handler = cmd_divzero,
    .description = "Prueba excepcion de division por cero",
    .usage = "divzero"
};

static struct Tcommand cmd_invopcode_s = {
    .name = "invopcode",
    .handler = cmd_invopcode,
    .description = "Prueba excepcion de opcode invalido",
    .usage = "invopcode"
};

static struct Tcommand cmd_exit_s = {
    .name = "exit",
    .handler = cmd_exit,
    .description = "Sale de la shell",
    .usage = "exit"
};

static struct Tcommand cmd_tron_s = {
    .name = "tron",
    .handler = cmd_tron,
    .description = "Jugar Tron (WASD vs IJKL, ESC para salir). Use 'tron fps' para mostrar FPS.",
    .usage = "tron [fps]"
};

static struct Tcommand cmd_beep_s = {
    .name = "beep",
    .handler = cmd_beep,
    .description = "Reproduce un sonido",
    .usage = "beep <frequency> <duration_ms>"
};

static struct Tcommand cmd_benchmark_s = {
    .name = "benchmark",
    .handler = cmd_benchmark,
    .description = "Ejecuta benchmarks de rendimiento(Use 'benchmark help' para mas informacion)",
    .usage = "benchmark [test_name|help]"
};

// Array de punteros a comandos (visible externamente)
Command commands[MAXCMD] = {
    &cmd_help_s,
    &cmd_time_s,
    &cmd_timezone_s,
    &cmd_inforeg_s,
    &cmd_clear_s,
    &cmd_scale_s,
    &cmd_divzero_s,
    &cmd_invopcode_s,
    &cmd_exit_s,
    &cmd_tron_s,
    &cmd_beep_s,
    &cmd_benchmark_s
};

// ============== FUNCIONES AUXILIARES ==============

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
static Command find_command(const char *name) {
    for (int i = 0; i < MAXCMD; i++) {
        if (commands[i] != NULL && commands[i]->name != NULL && strcmp(name, commands[i]->name) == 0) {
            return commands[i];
        }
    }
    return NULL;
}

//Parsea y ejecuta un comando
void parse_and_execute(char *input, int * isExit) {
    static char *argv_array[MAX_ARGS];
    static char last_input[MAX_COMMAND_LENGTH];
    static int retry_count = 0;
    
    // Tokenizar entrada
    int argc = tokenize(input, argv_array, MAX_ARGS);
    
    if (argc == 0) {
        return;  // Línea vacía
    }
    
    // Buscar comando
    Command cmd = find_command(argv_array[0]);
    
    if (cmd == NULL) {
        puts("Error: Comando no encontrado '", ERROR_COLOR);
        puts(argv_array[0], ERROR_COLOR);
        puts("'\n", ERROR_COLOR);
        puts("Escriba 'help' para ver comandos disponibles\n", INFO_COLOR);
        retry_count = 0;
        return;
    }
    
    // Guardar posición Y antes de ejecutar
    uint64_t coords_before = sys_get_coords();
    uint32_t y_before = (coords_before >> 32) & 0xFFFFFFFF;
    
    // Ejecutar handler
    cmd->handler(argc, argv_array);
    
    // Si el comando es exit, setear la flag
    if (strcmp(argv_array[0], "exit") == 0) {
        *isExit = 1;
        return;
    }
    
    // Obtener posición Y después de ejecutar
    uint64_t coords_after = sys_get_coords();
    uint32_t y_after = (coords_after >> 32) & 0xFFFFFFFF;
    
    // Si la posición Y volvió a 0, significa que hubo scroll/clear automático
    // y el comando perdió su output
    if (y_before > 100 && y_after < 50 && retry_count == 0 && strcmp(argv_array[0], "tron") != 0) {
        // Guardar el comando para reejecutar
        strcpy(last_input, input);
        retry_count = 1;
        
        // Limpiar pantalla y reejecutar
        sys_clear_screen();
        puts("[ Pantalla llena - Reejecutando comando ]\n\n", INFO_COLOR);
        
        // Reejecutar el comando con los mismos argumentos
        cmd->handler(argc, argv_array);
        retry_count = 0;
    } else {
        retry_count = 0;
    }
}

// ============== IMPLEMENTACIÓN DE COMANDOS ==============

static int cmd_help(int argc, char **argv) {
    puts("Comandos disponibles:\n", INFO_COLOR);
    size_t cmd_count = sizeof(commands) / sizeof(commands[0]);
    for (size_t i = 0; i < cmd_count; i++) {
        if (commands[i] != NULL && commands[i]->name != NULL) {
            printf(" - %s: %s\n", commands[i]->name, commands[i]->description);
        }
    }
    return 0;
}

static int cmd_time(int argc, char **argv) {
    uint8_t hours = getHours();
    uint8_t minutes = getMinutes();
    uint8_t seconds = getSeconds();
    uint8_t day = getDay();
    uint8_t month = getMonth();
    uint32_t year = getYear();

    printf("Fecha: %02d/%02d/%04d\n", day, month, year);
    printf("Hora: %02d:%02d:%02d\n", hours, minutes, seconds);
    int8_t offset = getTimezoneOffset();
    printf("Zona horaria: UTC%s%d\n", offset >= 0 ? "+" : "", -offset);
    return 0;
}

static int cmd_timezone(int argc, char **argv) {
    if (argc == 1) {
        // Sin argumentos: mostrar el offset actual
        int8_t offset = getTimezoneOffset();
        printf("Zona horaria actual: UTC%s%d\n", offset >= 0 ? "+" : "", -offset);
        printf("Uso: timezone <numero>\n");
        printf("Ejemplo: timezone -3  (para UTC-3, Argentina)\n");
        printf("         timezone -5  (para UTC-5, Colombia/Peru)\n");
        return 0;
    }
    
    if (argc != 2) {
        puts("Uso: timezone <numero>\n", ERROR_COLOR);
        return -1;
    }
    
    // Parsear el offset (puede ser negativo)
    int offset = atoi(argv[1]);
    
    if (offset < -12 || offset > 14) {
        puts("Error: Offset invalido. Debe estar entre -12 y +14.\n", ERROR_COLOR);
        return -1;
    }
    
    // Invertir el signo: usuario dice UTC-3, internamente guardamos +3 para restar
    setTimezoneOffset(-offset);
    
    printf("Zona horaria cambiada a UTC%s%d\n", offset >= 0 ? "+" : "", offset);
    puts("Ejecuta 'time' para ver la hora actualizada.\n", INFO_COLOR);
    return 0;
}

static int cmd_inforeg(int argc, char **argv) {
    #define TOTAL_REGS 17
    uint64_t registers[TOTAL_REGS];
    
    // Obtener registros capturados
    uint64_t result = sys_get_registers(registers);
    
    // En nuestro ABI: sys_get_registers retorna 0 en éxito, -1 en error.
    if (result != 0) {
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
        char buffer[32];
        uint64_t value = registers[i];
        
        // Convert to hex manually
        char hex[] = "0123456789ABCDEF";
        buffer[0] = '0';
        buffer[1] = 'x';
        for(int j = 0; j < 16; j++) {
            buffer[2+j] = hex[(value >> (60 - j*4)) & 0xF];
        }
        buffer[18] = '\0';
        
        printf("%s: %s", register_names[i], buffer);
        
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
    puts("Escala de fuente cambiada a ", INFO_COLOR);
    putcharColor('0' + scale, INFO_COLOR);
    puts(".\n", INFO_COLOR);
    return 0;
}

static int cmd_divzero(int argc, char **argv) {
    _divzero();
    return 0;
}

static int cmd_invopcode(int argc, char **argv) {
    _invalidopcode();
    return 0;
}

static int cmd_exit(int argc, char **argv) {
    puts("Saliendo de la shell...\n", INFO_COLOR);
    puts("Hasta luego!\n", COLOR_CYAN);
    // Loop infinito con halt para detener ejecución
    while(1) {
        __asm__ volatile ("hlt");
    }
    return 1;
}

static int cmd_tron(int argc, char **argv) {
    // Modo FPS opcional: "tron fps"
    if (argc >= 2 && strcmp(argv[1], "fps") == 0) {
        tron_set_show_fps(1);
    } else {
        tron_set_show_fps(0);
    }

    tron_game();
    tron_set_show_fps(0);
    sys_clear_screen();
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
    
    puts("Reproduciendo sonido...\n", INFO_COLOR);
    sys_play_sound(frequency, duration);
    puts("Sonido reproducido!\n", INFO_COLOR);
    
    return 0;
}

static int cmd_benchmark(int argc, char **argv) {
    if (argc == 1) {
        // Sin argumentos: ejecutar todos los benchmarks
        run_benchmarks();
    } else if (argc == 2) {
        if (strcmp(argv[1], "help") == 0) {
            benchmark_help();
        } else {
            // Ejecutar un benchmark específico
            run_specific_benchmark(argv[1]);
        }
    } else {
        puts("Uso: benchmark [test_name|help]\n", ERROR_COLOR);
        return -1;
    }
    return 0;
}
