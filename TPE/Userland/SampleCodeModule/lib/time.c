#include "../includes/syscalls.h"
#include "../includes/time.h"

// Ajuste de zona horaria (horas a restar de UTC)
// UTC-3 para Argentina
#define TIMEZONE_OFFSET 3

static uint8_t bcdToDec(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

uint8_t getSeconds() {
    return bcdToDec((uint8_t)_sys_getTime(SECONDS_ARG));
}

uint8_t getMinutes() {
    return bcdToDec((uint8_t)_sys_getTime(MINUTES_ARG));
}

uint8_t getHours() {
    uint8_t hours = bcdToDec((uint8_t)_sys_getTime(HOURS_ARG));
    
    // Ajustar zona horaria
    if (hours >= TIMEZONE_OFFSET) {
        hours -= TIMEZONE_OFFSET;
    } else {
        // Si la resta da negativo, ajustar (ej: 02:00 UTC - 3 = 23:00 del día anterior)
        hours = 24 + hours - TIMEZONE_OFFSET;
    }
    
    return hours;
}

uint8_t getDay() {
    return bcdToDec((uint8_t)_sys_getTime(DAY_ARG));
}

uint8_t getMonth() {
    return bcdToDec((uint8_t)_sys_getTime(MONTH_ARG));
}

uint32_t getYear() {
    return bcdToDec((uint8_t)_sys_getTime(YEAR_ARG)) + YEAR_REFERENCE;
}