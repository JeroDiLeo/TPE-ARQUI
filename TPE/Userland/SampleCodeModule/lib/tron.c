/*
 * =============================================================================
 * 🕹️  TRON LIGHT CYCLES GAME
 * =============================================================================
 * Juego tipo Tron para el TPE de Arquitectura de Computadoras
 * Dos jugadores compiten dejando rastros de luz. El primero en chocar pierde.
 * 
 * Controles:
 *   - Jugador 1 (Cyan):  W/A/S/D
 *   - Jugador 2 (Magenta): I/J/K/L
 *   - R: Reiniciar
 *   - Q/ESC: Salir
 * =============================================================================
 */

#include "tron.h"
#include "syscalls.h"
#include "io_utils.h"

// Variables globales para información de pantalla
static int SCREEN_WIDTH = 1024;
static int SCREEN_HEIGHT = 768;

// ============== FUNCIONES AUXILIARES ==============

/**
 * @brief Obtiene información de la pantalla
 */
static void getScreenDimensions(void) {
    uint64_t info = sys_get_screen_info();
    SCREEN_WIDTH = (info >> 32) & 0xFFFFFFFF;   // 32 bits superiores = ancho
    SCREEN_HEIGHT = info & 0xFFFFFFFF;          // 32 bits inferiores = alto
}

/**
 * @brief Dibuja un bloque en pantalla
 */
static void drawBlock(int x, int y, uint64_t color) {
    sys_draw_rectangle(color, x, y, TRON_BLOCK_SIZE, TRON_BLOCK_SIZE);
}

/**
 * @brief Verifica si una posición está dentro de los límites de la pantalla
 */
static int isInBounds(int x, int y, int grid_width, int grid_height) {
    return (x >= 0 && x < grid_width && y >= 0 && y < grid_height);
}

/**
 * @brief Verifica si una posición coincide con otra
 */
static int posEquals(Position *a, Position *b) {
    return (a->x == b->x && a->y == b->y);
}

/**
 * @brief Verifica si una posición colisiona con el rastro de un jugador
 */
static int checkTrailCollision(Position *pos, Player *player) {
    for (int i = 0; i < player->trail_length; i++) {
        if (posEquals(pos, &player->trail[i])) {
            return 1;
        }
    }
    return 0;
}

// ============== INICIALIZACIÓN ==============

void initTron(GameState *game) {
    // Obtener dimensiones de la pantalla
    getScreenDimensions();
    
    game->screen_width = SCREEN_WIDTH;
    game->screen_height = SCREEN_HEIGHT;
    game->grid_width = SCREEN_WIDTH / TRON_BLOCK_SIZE;
    game->grid_height = SCREEN_HEIGHT / TRON_BLOCK_SIZE;
    
    // Inicializar Jugador 1 (Cyan) - lado izquierdo
    game->player1.pos.x = game->grid_width / 4;
    game->player1.pos.y = game->grid_height / 2;
    game->player1.dir = DIR_RIGHT;
    game->player1.color = COLOR_P1_TRAIL;
    game->player1.alive = 1;
    game->player1.trail_length = 1;
    game->player1.trail[0] = game->player1.pos;
    
    // Inicializar Jugador 2 (Magenta) - lado derecho
    game->player2.pos.x = (game->grid_width * 3) / 4;
    game->player2.pos.y = game->grid_height / 2;
    game->player2.dir = DIR_LEFT;
    game->player2.color = COLOR_P2_TRAIL;
    game->player2.alive = 1;
    game->player2.trail_length = 1;
    game->player2.trail[0] = game->player2.pos;
    
    // Estado inicial del juego
    game->game_over = 0;
    game->winner = 0;
}

void resetTron(GameState *game) {
    initTron(game);
}

// ============== INPUT ==============

void handleInput(GameState *game) {
    char input[256];
    int read_count = sys_read(0, input, 255);  // Lee del buffer de teclado
    
    if (read_count <= 0) {
        return;  // No hay input disponible
    }
    
    // Procesar cada carácter leído
    for (int i = 0; i < read_count; i++) {
        char key = input[i];
        
        // Jugador 1 - WASD
        if (key == KEY_W && game->player1.dir != DIR_DOWN) {
            game->player1.dir = DIR_UP;
        } else if (key == KEY_S && game->player1.dir != DIR_UP) {
            game->player1.dir = DIR_DOWN;
        } else if (key == KEY_A && game->player1.dir != DIR_RIGHT) {
            game->player1.dir = DIR_LEFT;
        } else if (key == KEY_D && game->player1.dir != DIR_LEFT) {
            game->player1.dir = DIR_RIGHT;
        }
        
        // Jugador 2 - IJKL (flechas simuladas)
        else if (key == KEY_UP && game->player2.dir != DIR_DOWN) {
            game->player2.dir = DIR_UP;
        } else if (key == KEY_DOWN && game->player2.dir != DIR_UP) {
            game->player2.dir = DIR_DOWN;
        } else if (key == KEY_LEFT && game->player2.dir != DIR_RIGHT) {
            game->player2.dir = DIR_LEFT;
        } else if (key == KEY_RIGHT && game->player2.dir != DIR_LEFT) {
            game->player2.dir = DIR_RIGHT;
        }
        
        // Controles del juego
        else if (key == KEY_R && game->game_over) {
            resetTron(game);
        } else if (key == KEY_Q || key == KEY_ESC) {
            game->game_over = 1;
            game->winner = -1;  // Salida manual
        }
    }
}

// ============== COLISIONES ==============

int checkCollision(GameState *game, Player *player) {
    Position next_pos = player->pos;
    
    // Calcular siguiente posición según dirección
    switch (player->dir) {
        case DIR_UP:
            next_pos.y--;
            break;
        case DIR_DOWN:
            next_pos.y++;
            break;
        case DIR_LEFT:
            next_pos.x--;
            break;
        case DIR_RIGHT:
            next_pos.x++;
            break;
    }
    
    // Verificar colisión con bordes
    if (!isInBounds(next_pos.x, next_pos.y, game->grid_width, game->grid_height)) {
        return 1;
    }
    
    // Verificar colisión con rastro propio (excepto la cabeza actual)
    for (int i = 0; i < player->trail_length - 1; i++) {
        if (posEquals(&next_pos, &player->trail[i])) {
            return 1;
        }
    }
    
    // Verificar colisión con rastro del otro jugador
    Player *other = (player == &game->player1) ? &game->player2 : &game->player1;
    if (checkTrailCollision(&next_pos, other)) {
        return 1;
    }
    
    return 0;
}

// ============== UPDATE ==============

void updateTron(GameState *game) {
    if (game->game_over) {
        return;
    }
    
    // Verificar colisiones ANTES de mover
    int p1_collision = checkCollision(game, &game->player1);
    int p2_collision = checkCollision(game, &game->player2);
    
    // Determinar ganador
    if (p1_collision && p2_collision) {
        // Empate - ambos chocan simultáneamente
        game->player1.alive = 0;
        game->player2.alive = 0;
        game->game_over = 1;
        game->winner = 0;
        sys_play_sound(400, 200);  // Beep de empate
        return;
    } else if (p1_collision) {
        // Jugador 2 gana
        game->player1.alive = 0;
        game->game_over = 1;
        game->winner = 2;
        sys_play_sound(800, 300);  // Beep de victoria
        return;
    } else if (p2_collision) {
        // Jugador 1 gana
        game->player2.alive = 0;
        game->game_over = 1;
        game->winner = 1;
        sys_play_sound(800, 300);  // Beep de victoria
        return;
    }
    
    // Mover Jugador 1
    if (game->player1.alive) {
        switch (game->player1.dir) {
            case DIR_UP:    game->player1.pos.y--; break;
            case DIR_DOWN:  game->player1.pos.y++; break;
            case DIR_LEFT:  game->player1.pos.x--; break;
            case DIR_RIGHT: game->player1.pos.x++; break;
        }
        
        // Agregar nueva posición al rastro
        if (game->player1.trail_length < MAX_TRAIL_LENGTH) {
            game->player1.trail[game->player1.trail_length] = game->player1.pos;
            game->player1.trail_length++;
        }
    }
    
    // Mover Jugador 2
    if (game->player2.alive) {
        switch (game->player2.dir) {
            case DIR_UP:    game->player2.pos.y--; break;
            case DIR_DOWN:  game->player2.pos.y++; break;
            case DIR_LEFT:  game->player2.pos.x--; break;
            case DIR_RIGHT: game->player2.pos.x++; break;
        }
        
        // Agregar nueva posición al rastro
        if (game->player2.trail_length < MAX_TRAIL_LENGTH) {
            game->player2.trail[game->player2.trail_length] = game->player2.pos;
            game->player2.trail_length++;
        }
    }
}

// ============== RENDER ==============

void drawTron(GameState *game) {
    // Dibujar solo la NUEVA posición del Jugador 1 (no redibujar todo)
    if (game->player1.alive && game->player1.trail_length > 0) {
        int idx = game->player1.trail_length - 1;  // Última posición añadida
        int px = game->player1.trail[idx].x * TRON_BLOCK_SIZE;
        int py = game->player1.trail[idx].y * TRON_BLOCK_SIZE;
        drawBlock(px, py, game->player1.color);
    }
    
    // Dibujar solo la NUEVA posición del Jugador 2 (no redibujar todo)
    if (game->player2.alive && game->player2.trail_length > 0) {
        int idx = game->player2.trail_length - 1;  // Última posición añadida
        int px = game->player2.trail[idx].x * TRON_BLOCK_SIZE;
        int py = game->player2.trail[idx].y * TRON_BLOCK_SIZE;
        drawBlock(px, py, game->player2.color);
    }
}

void drawInitialTrail(GameState *game) {
    // Dibujar TODO el rastro (solo usar al iniciar/reiniciar)
    for (int i = 0; i < game->player1.trail_length; i++) {
        int px = game->player1.trail[i].x * TRON_BLOCK_SIZE;
        int py = game->player1.trail[i].y * TRON_BLOCK_SIZE;
        drawBlock(px, py, game->player1.color);
    }
    
    for (int i = 0; i < game->player2.trail_length; i++) {
        int px = game->player2.trail[i].x * TRON_BLOCK_SIZE;
        int py = game->player2.trail[i].y * TRON_BLOCK_SIZE;
        drawBlock(px, py, game->player2.color);
    }
}

void drawGameOver(GameState *game) {
    // Limpiar pantalla
    sys_clear_screen();
    
    // Dibujar mensaje de fin de juego
    const char *msg1 = "=== GAME OVER ===\n\n";
    const char *msg2;
    
    if (game->winner == -1) {
        msg2 = "Gracias por jugar!\n";
    } else if (game->winner == 0) {
        msg2 = "EMPATE!\n\n";
    } else if (game->winner == 1) {
        msg2 = "JUGADOR 1 (CYAN) GANO!\n\n";
    } else {
        msg2 = "JUGADOR 2 (MAGENTA) GANO!\n\n";
    }
    
    const char *msg3 = "Presiona R para reiniciar\n";
    const char *msg4 = "Presiona Q para salir\n";
    
    sys_write(1, msg1, 20, COLOR_TEXT);
    
    if (game->winner == 1) {
        sys_write(1, msg2, 25, COLOR_P1_TRAIL);
    } else if (game->winner == 2) {
        sys_write(1, msg2, 2, COLOR_P2_TRAIL);
    } else {
        sys_write(1, msg2, 20, COLOR_TEXT);
    }
    
    sys_write(1, msg3, 27, COLOR_TEXT);
    sys_write(1, msg4, 24, COLOR_TEXT);
}

// ============== GAME LOOP ==============

void runTronGame(GameState *game) {
    // Limpiar pantalla inicial
    sys_clear_screen();
    
    // Dibujar instrucciones iniciales
    const char *title = "=== TRON LIGHT CYCLES ===\n\n";
    const char *p1_controls = "Jugador 1 (CYAN): W/A/S/D\n";
    const char *p2_controls = "Jugador 2 (MAGENTA): I/J/K/L\n\n";
    const char *start_msg = "Presiona cualquier tecla para empezar...\n";
    
    sys_write(1, title, 28, COLOR_TEXT);
    sys_write(1, p1_controls, 27, COLOR_P1_TRAIL);
    sys_write(1, p2_controls, 32, COLOR_P2_TRAIL);
    sys_write(1, start_msg, 42, COLOR_TEXT);
    
    // Esperar input para empezar
    char dummy[2];
    while (sys_read(0, dummy, 1) <= 0) {
        sys_sleep(50);
    }
    
    // Limpiar pantalla y empezar el juego
    sys_clear_screen();
    
    // Dibujar posiciones iniciales de ambos jugadores
    drawInitialTrail(game);
    
    // Loop principal del juego
    while (!game->game_over || game->winner == 0 || game->winner == 1 || game->winner == 2) {
        // Procesar input
        handleInput(game);
        
        // Si se pidió salir, terminar
        if (game->winner == -1) {
            break;
        }
        
        // Si el juego terminó, mostrar pantalla de game over
        if (game->game_over) {
            drawGameOver(game);
            
            // Esperar input para reiniciar o salir
            while (game->game_over && game->winner != -1) {
                handleInput(game);
                sys_sleep(100);
            }
            
            // Si se reinició, limpiar pantalla y redibujar posiciones iniciales
            if (!game->game_over) {
                sys_clear_screen();
                drawInitialTrail(game);
            }
            
            continue;
        }
        
        // Actualizar lógica del juego
        updateTron(game);
        
        // Renderizar solo las nuevas posiciones (no borrar ni redibujar todo)
        drawTron(game);
        
        // Control de velocidad
        sys_sleep(TRON_SPEED);
    }
    
    // Limpiar pantalla al salir
    sys_clear_screen();
}

// ============== FUNCIÓN PRINCIPAL ==============

void tron_game(void) {
    // Usar variable estática para evitar stack overflow (GameState es muy grande)
    static GameState game;
    
    // Inicializar el juego
    initTron(&game);
    
    // Ejecutar el loop principal
    runTronGame(&game);
    
    // Mensaje de despedida
    const char *bye = "Gracias por jugar Tron!\n";
    sys_write(1, bye, 25, COLOR_TEXT);
}