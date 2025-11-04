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
        "TTTTTTTTTTTTTT  RRRRRRRRRRRR    OOOOOOOOOO    NNN      NNN",
        "TTTTTTTTTTTTTT  RRRRRRRRRRRR   OOOOOOOOOOOO   NNNN     NNN",
        "TTTTTTTTTTTTTT  RRRR    RRRR  OOOO    OOOO    NNNNN    NNN",
        "    TTTTTT      RRRR    RRRR  OOOO    OOOO    NNN NN   NNN",
        "    TTTTTT      RRRRRRRRRRRR  OOOO    OOOO    NNN  NN  NNN",
        "    TTTTTT      RRRRRRRRRR    OOOO    OOOO    NNN   NN NNN",
        "    TTTTTT      RRRR   RRRR   OOOO    OOOO    NNN    NNNNN",
        "    TTTTTT      RRRR    RRRR  OOOO    OOOO    NNN     NNNN",
        "    TTTTTT      RRRR     RRRR  OOOOOOOOOOOO   NNN      NNN",
        "    TTTTTT      RRRR      RRRR  OOOOOOOOOO    NNN      NNN"
    };

    for (int i = 0; i < 10; i++) {
        puts(lines[i], CYAN);
        puts("\n", CYAN); 
        _sys_sleep(150);
    }

}