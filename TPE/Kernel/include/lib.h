#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char *cpuVendor(char *result);

size_t strlen(const char *str);
void reverse(char *str);
//Convierte un numero entero a string en base decimal
char * itoaHex(uint64_t num, char * str); //Va en lib.c del userland

#endif