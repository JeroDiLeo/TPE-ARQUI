#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include <stdint.h>

/* Dibuja un pixel en (x,y) con color RGB hex (0xRRGGBB). */
void drawPixel(uint32_t hexColor, uint64_t x, uint64_t y);

/* Dibuja un caracter de la fuente en (x,y) con color de texto y fondo. */
void drawChar(char c, uint32_t charColor, uint32_t bgColor, uint64_t x, uint64_t y);

/* Dibuja un rectángulo sólido de tamaño (width x height) en (x,y). 
   Devuelve 1 si se pudo dibujar completo; 0 si excede pantalla. */
int drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t width, uint64_t height);

/* Limpia la pantalla con color negro. */
void clearScreen(void);

/* Devuelve el ancho de la pantalla en píxeles. */
uint16_t getScreenWidth(void);

/* Devuelve el alto de la pantalla en píxeles. */
uint16_t getScreenHeight(void);

/* Fija la escala de texto (entera). Devuelve 1 si es válida, 0 si no. */
int setScale(uint8_t newScale);

/* Devuelve la escala actual de texto. */
uint8_t getScale(void);

#endif /* VIDEO_DRIVER_H */