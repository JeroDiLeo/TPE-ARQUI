#ifndef SYSCALL_LIB_H
#define SYSCALL_LIB_H
#include <stdint.h>

#define REGS_COUNT 17

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






#endif