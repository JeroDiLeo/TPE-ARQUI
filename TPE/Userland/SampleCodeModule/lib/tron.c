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
#include "time.h"
#include "tronLogo.h"

// Variables globales para información de pantalla
static int SCREEN_WIDTH = 1024;
static int SCREEN_HEIGHT = 768;

// Flag para mostrar FPS en el HUD (configurable desde la shell)
static int SHOW_FPS = 0;

// Contadores para el cálculo de FPS (actualizados una vez por segundo)
static int FPS_LAST_VALUE = 0;
static int FPS_FRAMES_THIS_SECOND = 0;
static int FPS_PREV_SECOND = -1;

// Caché para evitar redibujar el HUD innecesariamente (reduce parpadeo)
static int CACHED_P1_SCORE = -1;
static int CACHED_P2_SCORE = -1;
static int CACHED_FPS = -1;
static int HUD_NEEDS_FULL_REDRAW = 1;

// ================= HUD / SCOREBOARD =================
// Altura reservada para el HUD (en píxeles)
#define HUD_HEIGHT_PX   (TRON_BLOCK_SIZE * 6)   // suficiente para dígitos 3x5
#define HUD_PADDING_PX  8

// Fuente de dígitos 3x5 (cada fila son 3 bits; 1 = pinta celda)
#define DIGIT_COLS 3
#define DIGIT_ROWS 5
static const uint8_t DIGITS[10][DIGIT_ROWS] = {
    {0b111, 0b101, 0b101, 0b101, 0b111}, // 0
    {0b010, 0b110, 0b010, 0b010, 0b111}, // 1
    {0b111, 0b001, 0b111, 0b100, 0b111}, // 2
    {0b111, 0b001, 0b111, 0b001, 0b111}, // 3
    {0b101, 0b101, 0b111, 0b001, 0b001}, // 4
    {0b111, 0b100, 0b111, 0b001, 0b111}, // 5
    {0b111, 0b100, 0b111, 0b101, 0b111}, // 6
    {0b111, 0b001, 0b001, 0b001, 0b001}, // 7
    {0b111, 0b101, 0b111, 0b101, 0b111}, // 8
    {0b111, 0b101, 0b111, 0b001, 0b111}  // 9
};

// Letras necesarias para "FPS:" en fuente 3x5
static const uint8_t GLYPH_F[DIGIT_ROWS]     = {0b111, 0b100, 0b111, 0b100, 0b100};
static const uint8_t GLYPH_P[DIGIT_ROWS]     = {0b111, 0b101, 0b111, 0b100, 0b100};
static const uint8_t GLYPH_S[DIGIT_ROWS]     = {0b111, 0b100, 0b111, 0b001, 0b111};
static const uint8_t GLYPH_COLON[DIGIT_ROWS] = {0b010, 0b000, 0b010, 0b000, 0b010};

// Dibuja un dígito 3x5 en (x, y) en píxeles
static void drawDigitPx(int x, int y, int digit, uint64_t color) {
    if (digit < 0 || digit > 9) return;
    const uint8_t *rows = DIGITS[digit];
    int cell = TRON_BLOCK_SIZE; // tamaño de la celda del dígito
    for (int r = 0; r < DIGIT_ROWS; r++) {
        for (int c = 0; c < DIGIT_COLS; c++) {
            if ((rows[r] >> (DIGIT_COLS - 1 - c)) & 1) {
                sys_draw_rectangle(color,
                                   x + c * cell,
                                   y + r * cell,
                                   cell,
                                   cell);
            } else {
                // fondo del HUD (para borrar artefactos al actualizar)
                sys_draw_rectangle(COLOR_BACKGROUND,
                                   x + c * cell,
                                   y + r * cell,
                                   cell,
                                   cell);
            }
        }
    }
}

// Dibuja un glifo 3xN (o 1xN para ':') en (x, y) en píxeles
static void drawGlyphPx(int x, int y, const uint8_t *rows, int cols, uint64_t color) {
    if (rows == 0 || cols <= 0) return;
    int cell = TRON_BLOCK_SIZE;
    for (int r = 0; r < DIGIT_ROWS; r++) {
        for (int c = 0; c < cols; c++) {
            if ((rows[r] >> (cols - 1 - c)) & 1) {
                sys_draw_rectangle(color,
                                   x + c * cell,
                                   y + r * cell,
                                   cell,
                                   cell);
            } else {
                sys_draw_rectangle(COLOR_BACKGROUND,
                                   x + c * cell,
                                   y + r * cell,
                                   cell,
                                   cell);
            }
        }
    }
}

// Dibuja un caracter de la palabra "FPS:" (y espacio) con 3x5 celdas
// Devuelve el ancho en píxeles utilizado
static int drawSmallCharPx(int x, int y, char ch, uint64_t color) {
    int cell = TRON_BLOCK_SIZE;
    int spacing = cell;
    switch (ch) {
        case 'F': drawGlyphPx(x, y, GLYPH_F, 3, color); return 3*cell + spacing;
        case 'P': drawGlyphPx(x, y, GLYPH_P, 3, color); return 3*cell + spacing;
        case 'S': drawGlyphPx(x, y, GLYPH_S, 3, color); return 3*cell + spacing;
        case ':': drawGlyphPx(x, y, GLYPH_COLON, 1, color); return 1*cell + spacing;
        case ' ': default:
            // espacio: sólo limpiar el área equivalente a 2 celdas de ancho
            sys_draw_rectangle(COLOR_BACKGROUND, x, y, 2*cell, DIGIT_ROWS*cell);
            return 2*cell + spacing;
    }
}

static int measureSmallTextPx(const char *text) {
    if (!text) return 0;
    int cell = TRON_BLOCK_SIZE;
    int spacing = cell;
    int width = 0;
    for (const char *p = text; *p; p++) {
        switch (*p) {
            case 'F':
            case 'P':
            case 'S':
                width += 3*cell + spacing; break;
            case ':':
                width += 1*cell + spacing; break;
            case ' ': default:
                width += 2*cell + spacing; break;
        }
    }
    if (width > 0) width -= spacing; // quitar último espaciado
    return width;
}

static void drawSmallTextPx(int x, int y, const char *text, uint64_t color) {
    if (!text) return;
    for (const char *p = text; *p; p++) {
        x += drawSmallCharPx(x, y, *p, color);
    }
}

static int digitsCount(int value){
    if (value <= 0) return 1;
    int n = 0;
    while (value) { n++; value /= 10; }
    return n;
}

static void drawNumberPx(int x, int y, int value, uint64_t color) {
    if (value < 0) value = 0;
    int cell = TRON_BLOCK_SIZE;
    int spacing = cell; // espacio entre dígitos

    // Convertir a dígitos en un buffer (máx 10 dígitos para int)
    int buf[10];
    int idx = 0;
    if (value == 0) {
        buf[idx++] = 0;
    } else {
        int v = value;
        int tmp[10];
        int k = 0;
        while (v > 0 && k < 10) { tmp[k++] = v % 10; v /= 10; }
        // invertir
        for (int i = k - 1; i >= 0; --i) buf[idx++] = tmp[i];
    }

    for (int i = 0; i < idx; i++) {
        drawDigitPx(x, y, buf[i], color);
        x += DIGIT_COLS * cell + spacing;
    }
}

static void drawHUD(GameState *game) {
    // Puntajes (basados en la longitud del rastro recorrido)
    int p1_score = (game->player1.trail_length > 0) ? (game->player1.trail_length - 1) : 0;
    int p2_score = (game->player2.trail_length > 0) ? (game->player2.trail_length - 1) : 0;

    // Solo redibujar si algo cambió o es la primera vez
    int p1_changed = (p1_score != CACHED_P1_SCORE);
    int p2_changed = (p2_score != CACHED_P2_SCORE);
    int fps_changed = (FPS_LAST_VALUE != CACHED_FPS);
    
    // Si nada cambió, no hacer nada (evita parpadeo)
    if (!HUD_NEEDS_FULL_REDRAW && !p1_changed && !p2_changed && !fps_changed) {
        return;
    }

    // Dibujar fondo del HUD solo en redibujado completo
    if (HUD_NEEDS_FULL_REDRAW) {
        sys_draw_rectangle(COLOR_BACKGROUND, 0, 0, SCREEN_WIDTH, HUD_HEIGHT_PX);
        HUD_NEEDS_FULL_REDRAW = 0;
    }

    int square = TRON_BLOCK_SIZE * 3; // 12px con BLOCK_SIZE=4
    int cell = TRON_BLOCK_SIZE;
    int spacing = cell;

    // ==== JUGADOR 1 ====
    if (p1_changed || CACHED_P1_SCORE == -1) {
        // Limpiar área del P1 (cuadrado + números)
        int p1_clear_width = square + (TRON_BLOCK_SIZE) + 150; // espacio generoso
        sys_draw_rectangle(COLOR_BACKGROUND, HUD_PADDING_PX, HUD_PADDING_PX, 
                          p1_clear_width, DIGIT_ROWS * cell);
        
        // Dibujar cuadrado de color
        sys_draw_rectangle(COLOR_P1_TRAIL, HUD_PADDING_PX, HUD_PADDING_PX, square, square);
        
        // Dibujar números de P1
        int p1_x = HUD_PADDING_PX + square + (TRON_BLOCK_SIZE);
        int p1_y = HUD_PADDING_PX;
        drawNumberPx(p1_x, p1_y, p1_score, COLOR_P1_TRAIL);
        
        CACHED_P1_SCORE = p1_score;
    }

    // ==== JUGADOR 2 ====
    if (game->num_players == 2 && (p2_changed || CACHED_P2_SCORE == -1)) {
        // Calcular posición de P2
        int p2_square_x = SCREEN_WIDTH - HUD_PADDING_PX - square;
        int nDigits = digitsCount(p2_score);
        int numberWidth = nDigits * (DIGIT_COLS * cell + spacing) - spacing;
        int p2_numbers_x = p2_square_x - (TRON_BLOCK_SIZE) - numberWidth;
        
        // Limpiar área del P2
        int p2_clear_width = square + (TRON_BLOCK_SIZE) + 150; // espacio generoso
        int p2_clear_x = SCREEN_WIDTH - HUD_PADDING_PX - p2_clear_width;
        sys_draw_rectangle(COLOR_BACKGROUND, p2_clear_x, HUD_PADDING_PX, 
                          p2_clear_width, DIGIT_ROWS * cell);
        
        // Dibujar cuadrado de color P2
        sys_draw_rectangle(COLOR_P2_TRAIL, p2_square_x, HUD_PADDING_PX, square, square);
        
        // Dibujar números de P2
        int p2_numbers_y = HUD_PADDING_PX;
        drawNumberPx(p2_numbers_x, p2_numbers_y, p2_score, COLOR_P2_TRAIL);
        
        CACHED_P2_SCORE = p2_score;
    }

    // ==== FPS (CENTRO) ====
    if (SHOW_FPS && (fps_changed || CACHED_FPS == -1)) {
        const char *label = "FPS:";
        int labelWidth = measureSmallTextPx(label);
        int nDigits = digitsCount(FPS_LAST_VALUE);
        int numberWidth = nDigits * (DIGIT_COLS * cell + spacing) - spacing;
        int totalWidth = labelWidth + spacing + numberWidth;

        int fps_x = (SCREEN_WIDTH - totalWidth) / 2;
        int fps_y = HUD_PADDING_PX;

        // Limpiar área del FPS (con margen extra para cambios en número de dígitos)
        int digitW = (DIGIT_COLS * cell + spacing);
        int clearWidth = labelWidth + spacing + 5 * digitW; // hasta 5 dígitos
        int clear_x = (SCREEN_WIDTH - clearWidth) / 2;
        sys_draw_rectangle(COLOR_BACKGROUND, clear_x, fps_y, clearWidth, DIGIT_ROWS * cell);

        // Dibujar FPS
        drawSmallTextPx(fps_x, fps_y, label, COLOR_TEXT);
        drawNumberPx(fps_x + labelWidth + spacing, fps_y, FPS_LAST_VALUE, COLOR_TEXT);
        
        CACHED_FPS = FPS_LAST_VALUE;
    }
}

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
 * @brief Dibuja los bordes del área de juego
 */
static void drawBorders(GameState *game) {
    // Color del borde: amarillo brillante (estilo Tron clásico)
    uint64_t border_color = 0xFFFF00;  // Amarillo brillante
    int border_thickness = TRON_BLOCK_SIZE;
    
    int game_area_width = game->grid_width * TRON_BLOCK_SIZE;
    int game_area_height = game->grid_height * TRON_BLOCK_SIZE;
    
    // Borde superior (debajo del HUD)
    sys_draw_rectangle(border_color, 
                      0, 
                      HUD_HEIGHT_PX, 
                      game_area_width, 
                      border_thickness);
    
    // Borde inferior
    sys_draw_rectangle(border_color, 
                      0, 
                      HUD_HEIGHT_PX + game_area_height - border_thickness, 
                      game_area_width, 
                      border_thickness);
    
    // Borde izquierdo
    sys_draw_rectangle(border_color, 
                      0, 
                      HUD_HEIGHT_PX, 
                      border_thickness, 
                      game_area_height);
    
    // Borde derecho
    sys_draw_rectangle(border_color, 
                      game_area_width - border_thickness, 
                      HUD_HEIGHT_PX, 
                      border_thickness, 
                      game_area_height);
}

/**
 * @brief Dibuja un bloque en pantalla
 */
static void drawBlock(int x, int y, uint64_t color) {
    // Desplazar el campo de juego hacia abajo para reservar el HUD arriba
    sys_draw_rectangle(color, x, y + HUD_HEIGHT_PX, TRON_BLOCK_SIZE, TRON_BLOCK_SIZE);
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
    // Reservamos HUD_HEIGHT_PX en la parte superior para el marcador
    game->grid_height = (SCREEN_HEIGHT - HUD_HEIGHT_PX) / TRON_BLOCK_SIZE;

    // Si no fue seteado, por defecto 2 jugadores
    if (game->num_players != 1 && game->num_players != 2) {
        game->num_players = 2;
    }
    
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

    // Ajustes para modo 1 jugador
    if (game->num_players == 1) {
        game->player2.alive = 0;
        game->player2.trail_length = 0;
    }
}

void resetTron(GameState *game) {
    initTron(game);
    // Resetear caché del HUD para forzar redibujado completo
    CACHED_P1_SCORE = -1;
    CACHED_P2_SCORE = -1;
    CACHED_FPS = -1;
    HUD_NEEDS_FULL_REDRAW = 1;
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
        
        // Controles del juego (ESC para salir, R para reiniciar)
        if (key == KEY_ESC) {
            game->game_over = 1;
            game->winner = -1;  // Salida manual
            continue;
        }
        
        if (key == KEY_R && game->game_over) {
            resetTron(game);
            continue;
        }
        
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
        
        // Jugador 2 - IJKL (solo en modo 2 jugadores)
        if (game->num_players == 2) {
            if (key == KEY_UP && game->player2.dir != DIR_DOWN) {
                game->player2.dir = DIR_UP;
            } else if (key == KEY_DOWN && game->player2.dir != DIR_UP) {
                game->player2.dir = DIR_DOWN;
            } else if (key == KEY_LEFT && game->player2.dir != DIR_RIGHT) {
                game->player2.dir = DIR_LEFT;
            } else if (key == KEY_RIGHT && game->player2.dir != DIR_LEFT) {
                game->player2.dir = DIR_RIGHT;
            }
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
    
    if (game->num_players == 1) {
        // Solo verificar a P1
        int p1_collision = checkCollision(game, &game->player1);
        if (p1_collision) {
            game->player1.alive = 0;
            game->game_over = 1;
            game->winner = 0;  // sin ganador en 1P
            sys_play_sound(400, 200);
            return;
        }
    } else {
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
    if (game->num_players == 2 && game->player2.alive) {
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
    if (game->num_players == 2 && game->player2.alive && game->player2.trail_length > 0) {
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
    
    if (game->num_players == 2) {
        for (int i = 0; i < game->player2.trail_length; i++) {
            int px = game->player2.trail[i].x * TRON_BLOCK_SIZE;
            int py = game->player2.trail[i].y * TRON_BLOCK_SIZE;
            drawBlock(px, py, game->player2.color);
        }
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
    } else if (game->num_players == 1) {
        msg2 = "Fin de la partida (1 Jugador)\n\n";
    } else if (game->winner == 0) {
        msg2 = "EMPATE!\n\n";
    } else if (game->winner == 1) {
        msg2 = "JUGADOR 1 (CYAN) GANO!\n\n";
    } else {
        msg2 = "JUGADOR 2 (MAGENTA) GANO!\n\n";
    }
    
    const char *msg3 = "Presiona R para reiniciar\n";
    const char *msg4 = "Presiona ESC para salir\n";
    
    puts(msg1, COLOR_TEXT);
    
    if (game->winner == 1) {
        puts(msg2, COLOR_P1_TRAIL);
    } else if (game->winner == 2) {
        puts(msg2, COLOR_P2_TRAIL);
    } else {
        puts(msg2, COLOR_TEXT);
    }
    
    puts(msg3, COLOR_TEXT);
    puts(msg4, COLOR_TEXT);
}

// ============== GAME LOOP ==============

void runTronGame(GameState *game) {
    // Limpiar pantalla inicial
    sys_clear_screen();
    
    print_tron_boot_logo();
    // Dibujar instrucciones iniciales
    const char *title = "\n\n=== TRON LIGHT CYCLES ===\n\n";
    const char *p1_controls = "Jugador 1 (CYAN): W/A/S/D\n";
    const char *p2_controls = "Jugador 2 (MAGENTA): I/J/K/L\n\n";
    const char *mode_msg = "Presiona 1 (1 jugador) o 2 (2 jugadores)\n";
    const char *exit_msg = "Presiona ESC para salir\n";
    
    sys_write(1, title, 28, COLOR_TEXT);
    sys_write(1, p1_controls, 27, COLOR_P1_TRAIL);
    sys_write(1, p2_controls, 32, COLOR_P2_TRAIL);
    puts(mode_msg, COLOR_TEXT);
    puts(exit_msg, COLOR_TEXT);
    
    // Selección de modo: esperar 1, 2 o ESC
    char sel = 0;
    while (1) {
        sel = (char)sys_wait_key();
        if (sel == '1' || sel == '2') break;
        if (sel == KEY_ESC) {
            // Salir sin iniciar el juego
            sys_clear_screen();
            return;
        }
    }
    game->num_players = (sel == '1') ? 1 : 2;
    
    // Comienza directo tras elegir modalidad (1P o 2P)
    
    // Limpiar pantalla y empezar el juego
    sys_clear_screen();
    
    // Re-inicializar según modo elegido y dibujar
    initTron(game);
    // Dibujar bordes del área de juego
    drawBorders(game);
    // Dibujar posiciones iniciales de ambos jugadores
    drawInitialTrail(game);
    // Dibujar marcador inicial
    drawHUD(game);
    
    // Inicializar contadores de FPS
    FPS_LAST_VALUE = 0;
    FPS_FRAMES_THIS_SECOND = 0;
    FPS_PREV_SECOND = getSeconds();
    
    // Resetear caché del HUD
    CACHED_P1_SCORE = -1;
    CACHED_P2_SCORE = -1;
    CACHED_FPS = -1;
    HUD_NEEDS_FULL_REDRAW = 1;

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
            
            // Esperar input para reiniciar o salir (bloqueante, sin interferencias)
            while (game->game_over && game->winner != -1) {
                char k = (char)sys_wait_key();
                if (k == KEY_R) {
                    resetTron(game);
                } else if (k == KEY_ESC) {
                    game->winner = -1;
                }
            }
            
            // Si se reinició, limpiar pantalla y redibujar posiciones iniciales
            if (!game->game_over) {
                sys_clear_screen();
                drawBorders(game);
                drawInitialTrail(game);
            }
            
            continue;
        }
        
        // Actualizar lógica del juego
        updateTron(game);
        
        // Renderizar solo las nuevas posiciones (no borrar ni redibujar todo)
        drawTron(game);
        // Actualizar HUD/Scoreboard
        drawHUD(game);
        
        // Control de velocidad
        sys_sleep(TRON_SPEED);

        // Actualizar FPS una vez por segundo usando el RTC
        if (SHOW_FPS) {
            int cur = getSeconds();
            if (cur == FPS_PREV_SECOND) {
                FPS_FRAMES_THIS_SECOND++;
            } else {
                // nuevo segundo (maneja wrap 59->0 por comparación simple)
                FPS_LAST_VALUE = FPS_FRAMES_THIS_SECOND;
                FPS_FRAMES_THIS_SECOND = 1; // cuenta este frame en el nuevo segundo
                FPS_PREV_SECOND = cur;
            }
        }
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

void tron_set_show_fps(int show) {
    SHOW_FPS = (show != 0);
}
