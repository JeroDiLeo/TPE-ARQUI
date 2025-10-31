#ifndef _VIDEODRIVER_H
#define _VIDEODRIVER_H
#include <stdint.h>


/* dado una coordenada dibuja un pixel 

*/
void drawPixel(uint32_t hexColor, uint64_t x, uint64_t y);
/* dado una coordenada te dibuja un char 

*/
void drawChar(char c, uint32_t charColor, uint32_t bgColor, uint64_t x, uint64_t y);
/*  dibuja un rectangulo en la pantalla con las coordinadas que se
    dan por parametros
*/
int drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t width, uint64_t height);

void clearScreen();
// devuelve el width de la pantalla en pixels
uint16_t getScreenWidth();
// devuelve el height de la pantalla en pixels
uint16_t getScreenHeight();
//mete la escala en la panralla
uint16_t setScale(uint64_t newScale);
//devuelve la escala actual 
uint8_t getScale();

#endif