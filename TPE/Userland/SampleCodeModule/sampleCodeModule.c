#include "shell.h"
#include "syscalls.h"



// Forward declarations
// extern void _sys_write(int fd, const char *buffer, int count, int color);

// Test syscall functionality before starting shell

int main() {
    // Simple initial message
    
    // If syscalls work, start the shell
    return shell_main();
}