#ifndef __AUDIODIVER_H
#define __AUDIODRIVER_H

#include <stdint.h>

// juega sonido con la frecuencia dada
// @param la frecuencia
uint64_t playSound(uint32_t frecuency);

//parar el sonido
uint64_t stopSound();


#endif