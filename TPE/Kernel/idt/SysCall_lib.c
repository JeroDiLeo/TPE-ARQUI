#include "include/SysCall_Lib.h"
#include "include/interrupts.h"
#include "videoDriver.h"
#include "KBDriver.h"
#include "audioDriver.h"
#include "font.h"
#include "time.h"

#define STDIN 0
#define STDOUT 1

#define TAB 4

#define WHITE 0x00FFFFFF
#define BLACK 0x00000000
#define DEFAULT_BG_COLOR 0x00000000

static uint16_t position_x = 0;
static uint16_t position_y = 0;
uint32_t bgColor = DEFAULT_BG_COLOR;

static int printSpecialChars(char c);

uint64_t sys_read(uint64_t fd, char * buffer, uint64_t size) {
    // aca fijarse, capaz que medio confuso pero poner if(!fd) seria lo mismo
    if(fd != STDIN){
        return 0;
    }
    for(uint64_t i = 0; i < size; i++){
        char c = kb_getchar();  //funcion dentro keyboard para dar el char 
        buffer[i] = c;
        if(c == 0){
            return i; // si no hay mas caracteres, devuelve la cantidad leida
        }
    }

    return size; //devuelve la cantidad leida
}

uint64_t sys_write(uint64_t fd, char * buffer, uint64_t size, uint64_t color){
    if(fd == STDOUT) {
        for(int i = 0; i < size; i++){ 
            if(buffer[i] != ESC){
                if((position_x + getFontWidth() * getScale()) > getScreenWidth()){
                    position_x = 0;
                    position_y += getFontHeight() * getScale();
                }

                if((position_y + getFontHeight() * getScale()) > getScreenHeight()){
                    //no hay mas espacion en la pantalla, devuelvo los chars escritos
                    return size;
                }

                if(!printSpecialChars(buffer[i])){
                    drawChar(buffer[i], color, bgColor, position_x, position_y);
                    position_x += getFontWidth() * getScale();
                }

            }
        }
        return size;
    }
    return 0;
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
        position_x += width * scale * TAB;
        return 1;
        case '\b':
        if(position_x > 0){
            position_x -= width * scale;
        }else if(position_y > 0){
            position_x = getScreenWidth() - width * scale;
            position_y -= height * scale;

            //alineamiento del cursor
            position_x -= position_x % (width * scale);
        }
        drawChar(' ', BLACK, bgColor, position_x, position_y);
        return 1;
        default:
        return 0;
    }
}

uint64_t sys_drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t width, uint64_t height){
    return drawRectangle(hexColor, x, y, width, height);
}

uint64_t sys_getCords(){
    return ((uint64_t) position_y << 32) | position_x;
}

uint64_t sys_clearScreen(){
    drawRectangle(bgColor, 0, 0, getScreenWidth(), getScreenHeight());
    position_x = 0;
    position_y = 0;
    return 0;
}

uint64_t sys_getScreenInfo(){
    return ((uint64_t) getScreenWidth() << 32) | getScreenHeight();
}

uint64_t sys_getFontInfo(){
    return ((uint64_t) (getFontWidth() * getScale()) << 32) | (getFontHeight() * getScale());
}

uint64_t sys_getTime(uint64_t arg){
    return getTime(arg);
}

uint64_t sys_setFontScale(uint64_t newScale){
    return setScale(newScale);
}

uint64_t sys_getRegisters(uint64_t * regs){
    return (uint64_t)getRegisters(regs);
}

uint64_t sys_sleep(uint64_t milisegundos){
    unsigned long long initialTime = ms_elapsed();
    unsigned long long currentTime = initialTime;

    _sti();
    while((currentTime - initialTime) <= milisegundos){
        currentTime = ms_elapsed();
    }

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