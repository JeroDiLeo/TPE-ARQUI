#ifndef _TIME_H
#define _TIME_H

#include <stdint.h>

// RTC register arguments
#define SECONDS_ARG 0x00
#define MINUTES_ARG 0x02
#define HOURS_ARG  0x04
#define DAY_ARG    0x07
#define MONTH_ARG  0x08
#define YEAR_ARG   0x09

#define YEAR_REFERENCE 2000

// Time getter functions
uint8_t getSeconds(void);
uint8_t getMinutes(void);
uint8_t getHours(void);
uint8_t getDay(void);
uint8_t getMonth(void);
uint32_t getYear(void);

#endif