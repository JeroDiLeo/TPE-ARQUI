#ifndef KBDRIVER_H
#define KBDRIVER_H

#include <stdint.h>

// Scan codes de teclas especiales 
#define LSHIFT          0x2A
#define RSHIFT          0x36
#define LCTRL           0x1D
#define RCTRL           0x1D  // (requiere E0 prefix, lo manejarías después)
#define CAPS            0x3A
#define LALT            0x38
#define RALT            0x38  // (requiere E0 prefix)

// Offset para release codes (cuando se suelta la tecla) (Es el flag )
#define RELEASE_OFFSET  0x80

// Release codes (Make code + 0x80)
#define LSHIFT_RELEASE  (LSHIFT + RELEASE_OFFSET)   // 0xAA
#define RSHIFT_RELEASE  (RSHIFT + RELEASE_OFFSET)   // 0xB6
#define LCTRL_RELEASE   (LCTRL + RELEASE_OFFSET)    // 0x9D
#define CAPS_RELEASE    (CAPS + RELEASE_OFFSET)     // 0xBA

// Teclas adicionales útiles
#define ENTER           0x1C
#define BACKSPACE       0x0E
#define TAB             0x0F
#define ESC             0x01
#define SPACE           0x39

#define KEYS_SIZE  59

// Cantidad de registros capturados por Ctrl+R
// No incluye RFLAGS, CS, SS (innecesarios en contexto de handler kernel)
#define TOTAL_REGS 17

// Funciones
void keyboard_check();
char keyboard_getchar();
void updateReg();
int getRegisters(uint64_t * r);

#endif // KBDRIVER_H