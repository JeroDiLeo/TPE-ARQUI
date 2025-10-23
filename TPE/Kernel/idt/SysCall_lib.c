#include "include/SysCall_Lib.h"
#include "include/interrupts.h"

#define STDIN 0
#define STDOUT 1

uint64_t sys_read(uint64_t fd, char * buffer, uint64_t size) {
    // aca fijarse, capaz que medio confuso pero poner if(!fd) seria lo mismo 
if(fd == STDIN) {
    for(int i = 0; i < size; i++){
        char c = kb_getchar(); //funcion dentro keyboard para dar el char 
        if(c == 0){
            return i; // si no hay mas caracteres, devuelve la cantidad leida
        }
        buffer[i] = c;
        }
    }
    return size; //devuelve la cantidad leida
}

uint64_t sys_write(uint64_t fd, char * buffer, uint64_t size, uint64_t color){
    if(fd == STDOUT) {
        for(int i = 0; i < size; i++){ 
            
        }
    }
 //devuelve la cantidad escrita
}