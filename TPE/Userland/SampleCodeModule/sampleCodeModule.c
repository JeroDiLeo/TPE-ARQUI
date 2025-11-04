#include "shell.h"
#include "syscalls.h"

int main() {
    shell_main();
    
    // Cuando la shell termina, llamar a sys_exit para notificar al kernel
    sys_exit(0);
    return 0;
}