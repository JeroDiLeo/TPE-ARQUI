#ifndef _COMMANDS_H
#define _COMMANDS_H

#define MAXCMD 10

// Declaración forward de la estructura Command
typedef struct Tcommand * Command;

// Array de comandos disponibles (definido en commands.c)
extern Command commands[MAXCMD];

// Función pública para parsear y ejecutar comandos
void parse_and_execute(char *input, int * exit);

#endif