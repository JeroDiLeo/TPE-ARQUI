#include <stdint.h>
#include "SysCall_Lib.h"

uint64_t syscallHandler(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t rax) {
    if(rax < 0 || rax > SYS_GETBGCOLOR){
        return -1;
    }

    switch (rax) {
        case SYS_READ: return sys_read(rdi, (char *) rsi, rdx);
        case SYS_WRITE: return sys_write(rdi, (char *) rsi, rdx, rcx);
        case SYS_RECTANGLE: return sys_drawRectangle(rdi, rsi, rdx, rcx, r8);
        case SYS_CLEAR: return sys_clearScreen();
        case SYS_COORDS: return sys_getCoords();
        case SYS_SCREENINFO: return sys_getScreenInfo();
        case SYS_FONTINFO: return sys_getFontInfo();
        case SYS_GETTIME: return sys_getTime(rdi);
        case SYS_SETSCALE: return sys_setFontScale(rdi);
        case SYS_GETREGS: return sys_getRegisters((uint64_t *) rdi);
        case SYS_SLEEP: return sys_sleep(rdi);
        case SYS_PLAYSOUND: return sys_playSound(rdi, rsi);
        case SYS_GETBGCOLOR : return sys_getBgColor();
        case SYS_EXIT : return sys_exit((int)rdi);
        default: return 0;
    }
}