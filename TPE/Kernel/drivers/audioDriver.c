#include "include/audioDriver.h"

extern void _outb(uint16_t port, uint8_t value);
extern uint8_t _inb(uint16_t port);

#define PIT_FREQ 1193182 //frecuencia standard de IBM PIT .... CHECKEAR 

uint64_t playSound(uint64_t frecuency){
    uint32_t div = PIT_FREQ / frecuency;

    // configuracion del PIT (Programmable Interval Timer)
    // se divide en 2 partes ya que el puerto es de 8 bits, partimos el div en 2 al ser 16 bits
    _outb(0x43, 0xB6);
    _outb(0x42, (uint8_t) (div & 0xFF)); //extraer los 8 bits bajos de div
    _outb(0x42, (uint8_t) ((div >> 8) & 0xFF)); //desplaza 8 bits para la derecha 

    //activar el sonido
    uint8_t aux = _inb(0x61);
    if(aux != (aux | 3)){
        _outb(0x61, aux | 3);
    }

    return 1;
}

uint64_t stopSound(){
    uint8_t aux = _inb(0x61) & 0xFC; //mando bits 0 y 1 a 0
    _outb(0x61, aux);
    return 1;
}