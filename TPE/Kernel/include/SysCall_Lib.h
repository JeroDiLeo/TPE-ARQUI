#ifndef SYSCALL_LIB_H
#define SYSCALL_LIB_H
#include <stdint.h>

#define TOTAL_REGS 17

// enum para los registros que se usaran para distinguir que syscall quiere el usario
typedef enum {
    rax = 0,
    rbx,
    rcx,
    rdx,                    
    rsi,
    rdi,
    rbp,
    r8,         
    r9,
    r10,
    r11,
    r12,
    r13,
    r14,
    r15,
    rip,
    rsp
} registers_t;

//Definicion de los syscalls para el handler
typedef enum {
    SYS_READ = 0,
    SYS_WRITE,
    SYS_RECTANGLE,
    SYS_CLEAR,
    SYS_COORDS,
    SYS_SCREENINFO,
    SYS_FONTINFO,
    SYS_GETTIME,
    SYS_SETSCALE,
    SYS_GETREGS,
    SYS_SLEEP,
    SYS_PLAYSOUND   
} SysCallID;

// int size en vez de uint64_t para los syscalls de read y write
int64_t sys_read(int fd, char * buffer, uint64_t size);

int64_t sys_write(int fd, const char * buffer, uint64_t size, uint64_t color);

int64_t sys_drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t width, uint64_t height);

int64_t sys_getCords();

int64_t sys_clearScreen();

int64_t sys_getScreenInfo();

int64_t sys_getFontInfo();

int64_t sys_getTime(uint64_t arg);

int64_t sys_setFontScale(uint64_t newScale);

int64_t sys_getRegisters(uint64_t * regs);

int64_t sys_sleep(uint64_t milisegundos);

int64_t sys_playSound(uint64_t f, uint64_t milisegundos);

int64_t sys_setBgColor(uint32_t newColor);

int64_t sys_getBgColor();


#endif