#include <stdint.h>
#include "syscalls.h"

// Simple test to verify syscalls work
int main() {
    // Test 1: Clear screen
    _sys_clearScreen();
    
    // Test 2: Write a simple message
    const char* msg1 = "Hello from userland!\n";
    const char* msg2 = "Syscalls are working!\n";
    const char* msg3 = "Type any key...\n";
    
    // Calculate lengths manually (no strlen yet)
    int len1 = 0;
    while(msg1[len1]) len1++;
    
    int len2 = 0;
    while(msg2[len2]) len2++;
    
    int len3 = 0;
    while(msg3[len3]) len3++;
    
    // Write messages with different colors
    _sys_write(1, msg1, len1, 0x00FF00);  // Green
    _sys_write(1, msg2, len2, 0x00FFFF);  // Cyan
    _sys_write(1, msg3, len3, 0xFFFF00);  // Yellow
    
    // Test 3: Try to read a character
    char buffer[2];
    _sys_read(0, buffer, 1);
    buffer[1] = '\n';
    
    // Echo it back
    const char* echo_msg = "You pressed: ";
    int echo_len = 0;
    while(echo_msg[echo_len]) echo_len++;
    
    _sys_write(1, echo_msg, echo_len, 0xFFFFFF);
    _sys_write(1, buffer, 2, 0xFF00FF);  // Magenta
    
    // Infinite loop - don't return
    while(1) {
        __asm__ __volatile__("hlt");
    }
    
    return 0;
}