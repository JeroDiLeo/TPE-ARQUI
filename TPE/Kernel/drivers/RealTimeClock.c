#include <RealTimeClock.h>

extern uint64_t _readTime(uint64_t param);

uint64_t getTime(uint64_t param) {
    return _readTime(param);
}
//retorna 64 bits  para compatibilidad con syscallHandler.c
//@return uint64_t Valor del componente (solo los 8 bits inferiores son válidos)