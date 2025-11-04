#ifndef _COMMANDS_H
#define _COMMANDS_H

// ============== CONSTANTES ==============
#define MAXCMD 12
#define MAX_COMMAND_LENGTH 256
#define MAX_ARGS 10

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

// Declaración forward de la estructura Command
typedef struct Tcommand * Command;

// Array de comandos disponibles (definido en commands.c)
extern Command commands[MAXCMD];

// Función pública para parsear y ejecutar comandos
void parse_and_execute(char *input, int * exit);

#endif