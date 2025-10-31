#ifndef LIB_H
#define LIB_H

#include <stdint.h>

// String functions
int strlen(const char *str);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, int n);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);

// Conversion functions
int atoi(const char *str);
uint64_t atoi_hex(const char *str);
void itoa(int value, char *buffer, int base);
void uitoa(uint64_t value, char *buffer, int base);
void itoa_hex(uint64_t value, char *buffer);

// Memory functions
void *memset(void *dest, int val, uint64_t count);
void *memcpy(void *dest, const void *src, uint64_t count);

// I/O helpers
void putchar(char c, uint64_t color);
void puts(const char *str, uint64_t color);
int getchar(void);
int read_line(char *buffer, int max_length);

// String formatting (simple sprintf)
void sprintf(char *buffer, const char *format, ...);

#endif
