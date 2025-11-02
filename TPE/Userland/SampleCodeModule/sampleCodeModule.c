#include "shell.h"
#include "syscalls.h"



// Forward declarations
extern void _sys_write(int fd, const char *buffer, int count, int color);

// Test syscall functionality before starting shell
static int test_syscall() {
    // Try to write a simple message using direct syscall
    _sys_write(1, "Entering test_syscall\n", 21, 0xFFFFFF);
    _sys_write(1, "Testing syscalls...\n", 19, 0xFFFFFF);
    return 0;  // Return success for now
}

int main() {
    // Simple initial message
    _sys_write(1, "Starting userland\n", 17, 0xFFFFFF);
    
    // Test syscalls
    int result = test_syscall();
    
    // Convert result to string and print it
    char result_str[32];
    result_str[0] = 'R';
    result_str[1] = 'e';
    result_str[2] = 's';
    result_str[3] = ':';
    result_str[4] = ' ';
    result_str[5] = result < 0 ? '-' : '+';
    result_str[6] = '\n';
    result_str[7] = 0;
    _sys_write(1, result_str, 7, 0xFFFFFF);
    
    if (result < 0) {
        return -1;
    }
    
    // If syscalls work, start the shell
    return shell_main();
}