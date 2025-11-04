#ifndef TRON_H
#define TRON_H

#include <stdint.h>

// ============== CONFIGURACIÓN DEL JUEGO ==============
#define TRON_BLOCK_SIZE 4           // Tamaño de cada bloque/píxel del rastro
#define TRON_SPEED 50               // Milisegundos entre movimientos (más bajo = más rápido)
#define MAX_TRAIL_LENGTH 10000      // Máxima longitud del rastro

// ============== COLORES ==============
#define COLOR_P1_TRAIL   0x00FFFF   // Cyan para Jugador 1
#define COLOR_P2_TRAIL   0xFF00FF   // Magenta para Jugador 2
#define COLOR_BACKGROUND 0x000000   // Negro
#define COLOR_WALL       0x404040   // Gris oscuro
#define COLOR_TEXT       0xFFFFFF   // Blanco

// ============== DIRECCIONES ==============
typedef enum {
    DIR_UP = 0,
    DIR_RIGHT = 1,
    DIR_DOWN = 2,
    DIR_LEFT = 3
} Direction;

// ============== TECLAS ==============
// Jugador 1: WASD
#define KEY_W 'w'
#define KEY_A 'a'
#define KEY_S 's'
#define KEY_D 'd'

// Jugador 2: Flechas (simuladas con IJKL en sistemas sin flechas directas)
#define KEY_UP 'i'
#define KEY_LEFT 'j'
#define KEY_DOWN 'k'
#define KEY_RIGHT 'l'

// Controles generales
#define KEY_ESC 0x01
#define KEY_R 'r'       // Reiniciar
#define KEY_Q 'q'       // Salir

// ============== ESTRUCTURAS ==============

// Posición en la pantalla
typedef struct {
    int x;
    int y;
} Position;

// Estado del jugador
typedef struct {
    Position pos;               // Posición actual
    Direction dir;              // Dirección actual
    uint64_t color;             // Color del rastro
    int alive;                  // 1 si está vivo, 0 si chocó
    Position trail[MAX_TRAIL_LENGTH];  // Historial del rastro
    int trail_length;           // Cantidad de posiciones en el rastro
} Player;

// Estado general del juego
typedef struct {
    Player player1;
    Player player2;
    int num_players;          // 1 = single player, 2 = two players
    int screen_width;
    int screen_height;
    int grid_width;             // Ancho en bloques
    int grid_height;            // Alto en bloques
    int game_over;              // 1 si el juego terminó
    int winner;                 // 0 = empate, 1 = jugador 1, 2 = jugador 2
} GameState;

// ============== FUNCIONES PÚBLICAS ==============

/**
 * @brief Función principal del juego Tron
 * Inicializa y ejecuta el loop principal del juego
 */
void tron_game(void);

/**
 * Habilita/deshabilita el overlay de FPS en el HUD.
 * Llamar con 1 para mostrar, 0 para ocultar.
 */
void tron_set_show_fps(int show);

/**
 * @brief Inicializa el estado del juego
 * @param game Puntero al estado del juego
 */
void initTron(GameState *game);

/**
 * @brief Actualiza la lógica del juego (movimiento, colisiones)
 * @param game Puntero al estado del juego
 */
void updateTron(GameState *game);

/**
 * @brief Dibuja el estado actual del juego en pantalla (solo nuevas posiciones)
 * @param game Puntero al estado del juego
 */
void drawTron(GameState *game);

/**
 * @brief Dibuja el rastro completo inicial (usar solo al iniciar/reiniciar)
 * @param game Puntero al estado del juego
 */
void drawInitialTrail(GameState *game);

/**
 * @brief Maneja la entrada del teclado
 * @param game Puntero al estado del juego
 */
void handleInput(GameState *game);

/**
 * @brief Verifica colisiones para un jugador
 * @param game Puntero al estado del juego
 * @param player Puntero al jugador a verificar
 * @return 1 si hay colisión, 0 si no
 */
int checkCollision(GameState *game, Player *player);

/**
 * @brief Reinicia el juego a su estado inicial
 * @param game Puntero al estado del juego
 */
void resetTron(GameState *game);

/**
 * @brief Ejecuta el loop principal del juego
 * @param game Puntero al estado del juego
 */
void runTronGame(GameState *game);

/**
 * @brief Dibuja la pantalla de fin de juego
 * @param game Puntero al estado del juego
 */
void drawGameOver(GameState *game);

#endif
