#include <RealTimeClock.h>

uint64_t getTime(uint64_t param) {
    return _readTime(param);
}
//Ver porque retorna 64 bits  si solo necesito 8 bits 