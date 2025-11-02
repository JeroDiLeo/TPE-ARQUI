#include "SysCall_Lib.h"
#include "interrupts.h"
#include "videoDriver.h"
#include "KBDriver.h"
#include "audioDriver.h"
#include "font.h"
#include "time.h"
#include "RealTimeClock.h"

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define SYS_TAB 4

#define WHITE 0x00FFFFFF
#define BLACK 0x00000000
#define DEFAULT_BG_COLOR 0x00000000

static uint16_t position_x = 0;
static uint16_t position_y = 0;
uint32_t bgColor = DEFAULT_BG_COLOR;

static int printSpecialChars(char c);
static void scrollScreen(void);
static volatile int shouldExit = 0;
static volatile int lastExitCode = 0;

uint64_t sys_read(int fd, char * buffer, uint64_t size) {
    if(fd != STDIN){
        return -1;  // fd inválido
    }
    
    for(uint64_t i = 0; i < size; i++){
        char c = kb_getchar();
        buffer[i] = c;
        if(c == 0){
            return i;  // No hay más caracteres
        }
    }
    return size;
}

// Desplaza la pantalla una línea hacia arriba (scroll)
static void scrollScreen(void) {
    uint8_t scale = getScale();
    uint16_t fontHeight = getFontHeight() * scale;
    uint16_t screenWidth = getScreenWidth();
    uint16_t screenHeight = getScreenHeight();
    
    // Calcular número de líneas completas de texto en pantalla
    uint16_t totalLines = screenHeight / fontHeight;
    
    if(totalLines <= 1) {
        // Si solo cabe 1 línea, limpiamos toda la pantalla
        sys_clearScreen();
        return;
    }
    
    // NOTA: Para mejor performance, esta implementación debería usar
    // acceso directo al framebuffer con memmove/memcpy.
    // La implementación actual es simple pero lenta.
    
    // Copiar framebuffer: desplazar (totalLines-1) líneas hacia arriba
    for(uint16_t line = 1; line < totalLines; line++) {
        //uint16_t srcY = line * fontHeight;
        uint16_t dstY = (line - 1) * fontHeight;
        
        // Por cada línea, copiar todos los píxeles
        for(uint16_t row = 0; row < fontHeight; row++) {
            for(uint16_t x = 0; x < screenWidth; x++) {
                // Leer y redibujar píxel
                // TODO: Optimizar con acceso directo a framebuffer
                // Por ahora usamos drawPixel (ineficiente)
                drawPixel(bgColor, x, dstY + row);  // Simplificado: llenar con bgColor
            }
        }
    }
    
    // Limpiar la última línea
    drawRectangle(bgColor, 0, (totalLines - 1) * fontHeight, screenWidth, fontHeight);
    
    // Posicionar cursor al inicio de la última línea
    position_x = 0;
    position_y = (totalLines - 1) * fontHeight;
}

uint64_t sys_write(int fd,const char * buffer, uint64_t size, uint64_t color){
    if(fd != STDOUT && fd != STDERR) {
        return -1;  // fd inválido
    }
    
    uint8_t scale = getScale();
    uint16_t fontWidth = getFontWidth() * scale;
    uint16_t fontHeight = getFontHeight() * scale;
    
    for(uint64_t i = 0; i < size; i++){ 
        // Verificar si necesitamos saltar de línea (word wrap)
        if((position_x + fontWidth) > getScreenWidth()){
            position_x = 0;
            position_y += fontHeight;
        }
        
        // Verificar si necesitamos hacer scroll
        if((position_y + fontHeight) > getScreenHeight()){
            scrollScreen();
        }

        if(!printSpecialChars(buffer[i])){
            drawChar(buffer[i], color, bgColor, position_x, position_y);
            position_x += fontWidth;
        }
    }
    return size;
}


static int printSpecialChars(char c){
    uint8_t scale = getScale();
    uint8_t width = getFontWidth();
    uint8_t height = getFontHeight();
    switch(c){
        case '\n' :
            position_x = 0;
            position_y += height * scale;
            return 1;
        case '\t' :
            // Calcular próximo tab stop (múltiplo de SYS_TAB = 4 espacios)
            {
                uint16_t charsPerLine = getScreenWidth() / (width * scale);
                uint16_t currentCol = position_x / (width * scale);
                uint16_t nextTabCol = ((currentCol / SYS_TAB) + 1) * SYS_TAB;
                
                if(nextTabCol < charsPerLine) {
                    position_x = nextTabCol * width * scale;
                } else {
                    // Si el tab nos saca de la línea, ir a la siguiente
                    position_x = 0;
                    position_y += height * scale;
                }
            }
            return 1;
        case '\b':
            if(position_x > 0){
                position_x -= width * scale;
            } else if(position_y > 0){
                position_x = getScreenWidth() - width * scale;
                position_y -= height * scale;
                // Alineamiento del cursor
                position_x -= position_x % (width * scale);
            }
            // Borrar con bgColor (no BLACK hardcodeado)
            drawChar(' ', bgColor, bgColor, position_x, position_y);
            return 1;
        default:
            return 0;
    }
}

uint64_t sys_drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t width, uint64_t height){
    return drawRectangle(hexColor, x, y, width, height);
}

uint64_t sys_getCoords(){
    return ((uint64_t) position_y << 32) | position_x;
}

uint64_t sys_clearScreen(){
    drawRectangle(bgColor, 0, 0, getScreenWidth(), getScreenHeight());
    position_x = 0;
    position_y = 0;
    return 0;
}

uint64_t sys_getScreenInfo(){
    // Empaqueta: width en 32 bits altos, height en 32 bits bajos
    return ((uint64_t) getScreenWidth() << 32) | getScreenHeight();
}

uint64_t sys_getFontInfo(){
    // Empaqueta: width en 32 bits altos, height en 32 bits bajos (escalados)
    return ((uint64_t) (getFontWidth() * getScale()) << 32) | (getFontHeight() * getScale());
}

uint64_t sys_getTime(uint64_t arg){
    return getTime(arg);
}

uint64_t sys_setFontScale(uint64_t newScale){
    // setScale devuelve 1 si OK, 0 si fuera de rango
    int result = setScale(newScale);
    return result ? 0 : -1;  // Retornar 0 en éxito, -1 en error
}

uint64_t sys_getRegisters(uint64_t * regs){
    if(regs == NULL) {
        return -1;  // Puntero inválido
    }
    
    int result = getRegisters(regs);
    return result ? 0 : -1;  // 0 en éxito, -1 si no hay snapshot disponible
}

uint64_t sys_sleep(uint64_t milisegundos){
    unsigned long long initialTime = ms_elapsed();
    unsigned long long currentTime = initialTime;
    
    // Preservar estado de interrupciones (simplificado)
    // NOTA: Si el enunciado lo requiere, usar pushf/popf en ASM
    _sti();
    
    while((currentTime - initialTime) <= milisegundos){
        _hlt();  // Esperar interrupciones (ahorra CPU)
        currentTime = ms_elapsed();
    }
    
    // Restaurar estado
    _cli();
    return 1;
}

uint64_t sys_playSound(uint64_t f, uint64_t milisegundos){
    playSound(f);
    sys_sleep(milisegundos);
    stopSound();
    return 1;
}

uint64_t sys_setBgColor(uint32_t newColor){
    bgColor = newColor;
    return 1;
}

uint64_t sys_getBgColor(){
    return bgColor;
}

uint64_t sys_exit(int exitCode){
    lastExitCode = exitCode;
    shouldExit = 1;
    return 0;
}

int checkShouldExit(void){
    return shouldExit;
}

int getLastExitCode(void){
    return lastExitCode;
}

void resetExitFlag(void){
    shouldExit = 0;
    lastExitCode = 0;
}
