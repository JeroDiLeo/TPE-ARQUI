#include "shell.h"
#include "syscalls.h"
#include "tronLogo.h"


int main() {
    // Start shell directly without testing syscalls first
    shell_main();
    
    // Cuando la shell termina, llamar a sys_exit para notificar al kernel
    sys_exit(0);
    
    // Loop infinito en caso de que sys_exit no detenga la ejecución
    // while(1) {
    //     __asm__ volatile ("hlt");
    // }
    
    return 0;
}