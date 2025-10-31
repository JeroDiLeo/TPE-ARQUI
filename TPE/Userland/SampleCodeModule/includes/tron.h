#ifndef TRON_H
#define TRON_H

#include <stdint.h>

// Configuración del juego
#define TRON_BLOCK_SIZE 8
#define TRON_SPEED 100  // milisegundos entre movimientos

// Direcciones
typedef enum {
    DIR_UP = 0,
    DIR_RIGHT = 1,
    DIR_DOWN = 2,
    DIR_LEFT = 3
} Direction;

// Teclas
#define KEY_W 'w'
#define KEY_A 'a'
#define KEY_S 's'
#define KEY_D 'd'
#define KEY_UP 'i'
#define KEY_LEFT 'j'
#define KEY_DOWN 'k'
#define KEY_RIGHT 'l'
#define KEY_ESC 27

// Función principal del juego
void tron_game(void);

#endif
