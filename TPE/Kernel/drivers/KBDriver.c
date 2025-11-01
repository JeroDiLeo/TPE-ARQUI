#include "KBDriver.h"
#include "SysCall_Lib.h"
#include <stddef.h>

extern uint8_t _getKeyScan();
extern uint64_t * _getRegs();
extern void _updateReg();

#define buffer_size 256

static char bufferPop(void);
static char bufferPush(char c);
static char bufferFull(void);
static char bufferEmpty(void);
static char scancodeToAscii(uint8_t scancode);
static void checkFlags(uint8_t scancode);


static char KeyCodeConversion[][2]={
    {0, 0}, {ESC, ESC}, {'1', '!'}, {'2', '@'}, {'3', '#'},
        {'4', '$'}, {'5', '%'}, {'6', '^'}, {'7', '&'}, {'8', '*'},
        {'9', '('}, {'0', ')'}, {'-', '_'}, {'=', '+'}, {'\b', '\b'},
        {'\t', '\t'}, {'q', 'Q'}, {'w', 'W'}, {'e', 'E'}, {'r', 'R'},
        {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'}, {'o', 'O'},
        {'p', 'P'}, {'[', '{'}, {']', '}'}, {'\n', '\n'}, {0, 0},
        {'a', 'A'}, {'s', 'S'}, {'d', 'D'}, {'f', 'F'}, {'g', 'G'},
        {'h', 'H'}, {'j', 'J'}, {'k', 'K'}, {'l', 'L'}, {';', ':'},
        {'\'', '\"'}, {'`', '~'}, {0, 0}, {'\\', '|'}, {'z', 'Z'},
        {'x', 'X'}, {'c', 'C'}, {'v', 'V'}, {'b', 'B'}, {'n', 'N'},
        {'m', 'M'}, {',', '<'}, {'.', '>'}, {'/', '?'}, {0, 0}, {0, 0},
        {0, 0}, {' ', ' '}
};

typedef struct Tbuffer{
    char bufferArr[buffer_size];
    int read;
    int write;
    int size;
} Tbuffer;

static Tbuffer buffer = { 
    .read = 0,
    .write = 0,
    .size = 0
};

//Flags para teclas especiales
static int ShiftOn = 0;
static int CapsOn = 0;
static int CtrlOn = 0;
static char fullRegisters = 0;
static uint64_t registers[TOTAL_REGS];


void keyboard_check(){
    uint8_t scancode = _getKeyScan();
    if(scancode == 0){
        return;
    }
    checkFlags(scancode);
    char asciiCode = scancodeToAscii(scancode);
    if(CtrlOn && (asciiCode == 'r' || asciiCode == 'R')){
        fullRegisters = 1;
        updateReg();
    }
    bufferPush(asciiCode);
}

char kb_getchar(){
    return bufferPop();
}

static char bufferPop(){
    if(bufferEmpty()){
        return 0;
    }

    char c = buffer.bufferArr[buffer.read];
    buffer.read = (buffer.read + 1) % buffer_size;
    buffer.size--;
    return c;
}

// pushea un char en el buffer y retorna 1 si pusheo 0 si no se pudo (buffer lleno)
static char bufferPush(char c){
    if(bufferFull()){
        return 0;
    }
    buffer.bufferArr[buffer.write] = c;
    buffer.write = (buffer.write + 1) % buffer_size;
    buffer.size++;
    return 1;
}

static char bufferFull(){
    return buffer.size == buffer_size;
}

static char bufferEmpty(){
    return buffer.read == buffer.write;
}


static char scancodeToAscii(uint8_t scancode){
    char scanCodeAscii = 0;
    if(scancode < KEYS_SIZE){
        scanCodeAscii = KeyCodeConversion[scancode][ShiftOn];
        if(CapsOn && scanCodeAscii >= 'a' && scanCodeAscii <= 'z'){
            scanCodeAscii -= 32; // la resta para obtener el ascii del char mayuscula
        }
    }
    return scanCodeAscii;
}

//check si usuario uso ctrl, caps o shift
static void checkFlags(uint8_t scancode){
    if(scancode == LCTRL){
        CtrlOn = 1;
    }else if(scancode == LCTRL_RELEASE){
        CtrlOn = 0;
    }else if(scancode == LSHIFT || scancode == RSHIFT){
        ShiftOn = 1;
    }else if(scancode == LSHIFT_RELEASE || scancode == RSHIFT_RELEASE){
        ShiftOn = 0;
    }else if(scancode == CAPS){
        CapsOn = !CapsOn;
    }
}

//actualiza el vector de los reguistros usando func de asm que copia el estado de los regitros 
void updateReg(){
    _updateReg();
    uint64_t * Regs = _getRegs();

    for(int i = 0; i < TOTAL_REGS; i++){
        registers[i] = Regs[i];
    }
}


int getRegisters(uint64_t * r){
    if(r == NULL || !fullRegisters){
        return 0;
    }

    for(int i = 0; i < TOTAL_REGS; i++){
        r[i] = registers[i];
    }
    return 1;
}

