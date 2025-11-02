#include "shell.h"
#include "syscalls.h"

// Función simple para probar syscalls
static int test_syscall() {
    // Intentar escribir un mensaje simple
    char *test = "Testing syscalls...\n";
    return _sys_write(1, test, 19, 0xFFFFFF);
}

int main() {
    // Start shell directly without testing syscalls first
    return shell_main();
}