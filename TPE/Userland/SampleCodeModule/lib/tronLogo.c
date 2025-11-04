// tronBootLogo.c
#include "syscalls.h"
#include "io_utils.h"

#define CYAN 0x00FFFF
#define MAGENTA 0xFF00FF
#define WHITE 0xFFFFFF
#define GRAY 0xAAAAAA

void print_tron_boot_logo(void) {
    _sys_clearScreen();

    const char *lines[] = {
        "TTTTTTTT  RRRRRR     OOOO    N    N",
        "   TT     RR   RR   OO  OO   NN   N",
        "   TT     RRRRRR   OO    OO  N N  N",
        "   TT     RR  RR   OO    OO  N  N N",
        "   TT     RR   RR   OO  OO   N   NN",
        "   TT     RR    RR   OOOO    N    N"
    };

    for (int i = 0; i < 6; i++) {
        puts(lines[i], CYAN);
        puts("\n", CYAN); 
        _sys_sleep(150);
    }

    _sys_sleep(500);
    puts("\n       ** TRON - Booting Up **\n\n", MAGENTA);

    for (int j = 0; j < 3; j++) {
        puts(".", WHITE);
        _sys_sleep(300);
    }

    puts("\n\n", WHITE);
    _sys_sleep(200);
    puts("Arquitectura de Computadoras - TPE 2025\n", GRAY);
    _sys_sleep(300);
    puts("Listo para jugar mi Fede hermoso?! \n\n", WHITE);

    _sys_sleep(500);
}