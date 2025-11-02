#include <SysCall_Lib.h>
#include <videoDriver.h>
#include <interrupts.h>
#include <lib.h>

#define INVALID_OPCODE_EXCEPTION 6
#define ZERO_EXCEPTION 0

#define BGCOLOR 0x00000000 // Black
#define WHITE 0x00FFFFFF // White

//Funcion asm que me guarde los registros en un array para el momento de la excepcion
extern uint64_t * _getSnapshot();

static void launchExceptionScreen(int exceptionType, const char * msg);
static void dumpRegisters(void);

//Maneja la excepcion segun su tipo
void exceptionHandler(int exceptionType) {
    char * msg = "Exception: Unknown\n";
    if(exceptionType == INVALID_OPCODE_EXCEPTION) {
        msg = "Exception: Invalid Opcode\n";
    } else if(exceptionType == ZERO_EXCEPTION) {
        msg = "Exception: Division by Zero\n";
    }
    launchExceptionScreen(exceptionType, msg);
}

//Muestra la pantalla de excepcion con el mensaje correspondiente
void launchExceptionScreen(int exceptionType, const char * msg) {
    //Guardo el estado visual actual(colores, escala)
    uint64_t oldBgColor = sys_getBgColor();
    uint8_t oldScale = getScale();

    //Seteo los nuevos parametros visuales para la pantalla de excepcion
    sys_setBgColor(BGCOLOR);
    setScale(2);
    sys_clearScreen();//Limpio la pantalla
    sys_write(1, msg, strlen(msg), WHITE);
    dumpRegisters();

    char * NextMsg = "\nPress any key to relaunch shell...";
    sys_write(1, NextMsg , strlen(NextMsg), WHITE);

    //Habilito las IRQs (teclado) para esperar el input del usuario
    _sti();
    char c;
    int readBytes = 0;
    while(readBytes == 0){
        readBytes = sys_read(0, &c, 1);
    }
    //Restauro la UI
    sys_setBgColor(oldBgColor);
    sys_clearScreen();
    setScale(oldScale);
}

//Imprime el valor de los registros guardados en el snapshot
void dumpRegisters(void) {

    static char * regNames[] = {
        "RAX   ", "RBX   ", "RCX   ", "RDX   ", "RSI   ", "RDI   ", "RBP   ",
        "R8    ", "R9    ", "R10   ", "R11   ", "R12   ", "R13   ", "R14   ",
        "R15   ", "RIP   ", "RSP   ", "RFLAGS", "CS    ", "SS    "
    };

    //Cantidad de registros guardados en el snapshot
    static int regsAmount = (sizeof(regNames) / sizeof(regNames[0]));
    uint64_t * regs = _getSnapshot();
    char buffer[17];

    for(int i = 0; i < regsAmount; i++) {
        //Convierte el valor del registro a hexadecimal (esta funcion iria en el userland lib.c)
        itoaHex(regs[i], buffer);
        // Agrega padding
        int pad=16 - strlen(buffer);
        //Imprime el nombre del registro y su valor
        sys_write(1, regNames[i], strlen(regNames[i]), WHITE);
        
        sys_write(1, ": 0x", 4, WHITE);
        //Imprime los 0s faltantes para completar 16 caracteres
        for(int j = 0; j < pad; j++) {
            sys_write(1, "0", 1, WHITE);
        }
        //Imprime el valor del registro
        sys_write(1, buffer, strlen(buffer), WHITE);
        sys_write(1, "\n", 1, WHITE);

    }
}
