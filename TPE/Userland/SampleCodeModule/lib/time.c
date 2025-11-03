#include "../includes/syscalls.h"
#include "../includes/time.h"

// Variable global para el offset de zona horaria (default UTC-3 para Argentina)
static int8_t timezone_offset = 3;

static uint8_t bcdToDec(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void setTimezoneOffset(int8_t offset) {
    // Validar que el offset esté en un rango razonable (-12 a +14)
    if (offset >= -12 && offset <= 14) {
        timezone_offset = offset;
    }
}

int8_t getTimezoneOffset(void) {
    return timezone_offset;
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
    int8_t adjusted = (int8_t)hours - timezone_offset;
    
    // Normalizar a rango 0-23
    if (adjusted < 0) {
        adjusted += 24;
    } else if (adjusted >= 24) {
        adjusted -= 24;
    }
    
    return (uint8_t)adjusted;
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