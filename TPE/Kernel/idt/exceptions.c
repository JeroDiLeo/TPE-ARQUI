#include <SysCall_Lib.h>
#include <interrupts.h>
#include <lib.h>

#define INVALID_OPCODE_EXCEPTION 6
#define ZERO_EXCEPTION 0

//Funcion asm que me guarde los registros  en un array para el momento de la excepcion
extern unit64_t * _getSnapshot();

void exceptionHandler(int exceptionType) {
    char * msg;
    if(exceptionType == INVALID_OPCODE_EXCEPTION) {
        msg = "Exception: Invalid Opcode\n";
    } else if(exceptionType == ZERO_EXCEPTION) {
        msg = "Exception: Division by Zero\n";
    }
    launchExceptionScreen(msg);
}
