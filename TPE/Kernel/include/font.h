#ifndef _FONT_H
#define _FONT_H

#include <stdint.h>
#include <stddef.h>


//func de info

//devuelve ancho de la fuente (pixeles)
uint8_t getFontWidth();

//devuelve el largo de la fuente (pixeles)
uint8_t getFontHeight();

//func de impresion


//devuelve ptero a la representacion de un char de la fuente (en el caso que no este en la fuente devuelve NULL)
uint8_t * getFontChar(char c);

#endif