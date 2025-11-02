#ifndef _COMMANDS_H
#define _COMMANDS_H

// Declaración forward de la estructura Command
typedef struct Tcommand Command;

// Función pública para parsear y ejecutar comandos
void parse_and_execute(char *input);

#endif