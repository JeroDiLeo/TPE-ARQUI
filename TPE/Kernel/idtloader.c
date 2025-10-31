#include "idtloader.h"
#include "interrupts.h"
#include <stdint.h>
#include "defs.h"

#pragma pack(push)		
#pragma pack (1) 		


typedef struct {
  uint16_t offset_l, selector;
  uint8_t cero, access;
  uint16_t offset_m;
  uint32_t offset_h, other_cero;
} DESCR_INT;


DESCR_INT * idt = (DESCR_INT *) 0;	


// excepciones
extern void exception_divideByZero(void);
extern void exception_invalidOpCode(void);
// Interrupciones de hardware
extern void interrupt_keyboard(void);
extern void interrupt_timerTick(void);
// syscalls
extern void interrupt_syscall();

static void setup_IDT_entry (int index, uint64_t offset);

void load_idt() {
  _cli();

  //Interrupciones de Hardware
  setup_IDT_entry (0x21, (uint64_t)&interrupt_keyboard); 
  setup_IDT_entry (0x20, (uint64_t)&interrupt_timerTick); 

  //Interrupciones de Software
  setup_IDT_entry (0x80, (uint64_t)&interrupt_syscall); 

  //Excepciones
  setup_IDT_entry (0x00, (uint64_t)&exception_divideByZero);
  setup_IDT_entry (0x06, (uint64_t)&exception_invalidOpCode);


  //Solo interrupcion timer tick habilitadas
  picMasterMask(0xFC);
  picSlaveMask(0xFF);
        
  _sti();
}

static void setup_IDT_entry (int index, uint64_t offset) {
  idt[index].selector = 0x08;
  idt[index].offset_l = offset & 0xFFFF;
  idt[index].offset_m = (offset >> 16) & 0xFFFF;
  idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
  idt[index].access = ACS_INT;
  idt[index].cero = 0;
  idt[index].other_cero = (uint64_t) 0;
}