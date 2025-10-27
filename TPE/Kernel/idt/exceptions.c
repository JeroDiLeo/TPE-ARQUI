#include <SysCall_Lib.h>
#include <interrupts.h>
#include <lib.h>

#define INVALID_OPCODE_EXCEPTION 6
#define ZERO_EXCEPTION 0

//Funcion asm que me guarde los registros en un array para el momento de la excepcion
extern uint64_t * _getSnapshot();

void exceptionHandler(int exceptionType) {
    char * msg;
    if(exceptionType == INVALID_OPCODE_EXCEPTION) {
        msg = "Exception: Invalid Opcode\n";
    } else if(exceptionType == ZERO_EXCEPTION) {
        msg = "Exception: Division by Zero\n";
    }
    launchExceptionScreen(msg);
}


//Ver que onda los ultimos 3 registros
static char * regNames[] = {
    "RAX   ", "RBX   ", "RCX   ", "RDX   ", "RSI   ", "RDI   ", "RBP   ",
    "R8    ", "R9    ", "R10   ", "R11   ", "R12   ", "R13   ", "R14   ",
    "R15   ", "RIP   ", "RSP   ", "RFLAGS", "CS    ", "SS    "
};

//Cantidad de registros guardados en el snapshot
static int regsAmount = (sizeof(regNames) / sizeof(regNames[0]));

void exceptionDispatcher(int exception) {
    char * message;
    if (exception == ZERO_EXCEPTION_ID)
        message = "ERROR 0X00: Division by Zero Exception\n";
    else if (exception == OPCODE_EXCEPTION_ID){
        message = "ERROR 0X06: Invalid Opcode Exception\n";
    }
    launchException(message);
}
