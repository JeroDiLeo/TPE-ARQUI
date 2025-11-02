#ifndef INTERRUPS_H_
#define INTERRUPS_H_

#include <stdint.h>

// Funciones ASM exportadas
void _cli(void);                    // Deshabilitar interrupciones
void _sti(void);                    // Habilitar interrupciones
void _hlt(void);                    // Sleep con interrupciones
void haltcpu(void);                 // Detener CPU permanentemente

void picMasterMask(uint8_t mask);   
void picSlaveMask(uint8_t mask);    

uint64_t * _getSnapshot(void);      // Obtener snapshot de registros
uint64_t _readCR2(void);            // Leer dirección causante de #PF

// Handlers de excepciones e interrupciones
void exception_divideByZero(void);
void exception_invalidOpCode(void);
void exception_pageFault(void);
void interrupt_timerTick(void);
void interrupt_keyboard(void);
void interrupt_syscall(void);

#endif
