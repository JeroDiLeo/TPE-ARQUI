#include "shell.h"
#include "syscalls.h"

int main() {
    // Start shell directly without testing syscalls first
    return shell_main();
}