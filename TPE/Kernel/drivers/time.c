#include "time.h"
#include <stdint.h>

#define ticks_seconds 18
#define ticks_ms 55

static unsigned long ticks = 0;

void timer_handler(){
    ticks++;
}
int ticks_elapsed(){
    return ticks;
}
int seconds_elapsed(){
    return (ticks / ticks_seconds);
}
uint64_t ms_elapsed(){
    return (ticks * ticks_ms);
}