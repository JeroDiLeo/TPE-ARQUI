#include "time.h"
#include "KBDriver.h"

#define PIC1_CMD   0x20
#define PIC1_DATA  0x21
#define PIC2_CMD   0xA0
#define PIC2_DATA  0xA1
#define PIC_EOI    0x20
#define IRQ_TIMER    0
#define IRQ_KEYBOARD 1

extern void _outbyte(uint16_t port, uint8_t value);
static void int_20h(void);
static void int_21h(void);
static void sendEOI(uint64_t irq);

void irqDispatcher(uint64_t irq) {
  switch (irq) {
    case IRQ_TIMER: int_20h();
    break;
    case IRQ_KEYBOARD: int_21h();
    break;

  }
  sendEOI(irq);
}

static void int_20h() {
    timer_handler();
}

static void int_21h() {
    keyboard_check();
}

static void sendEOI(uint64_t irq) {
    // IRQs 8-15 vienen del PIC esclavo, necesitan EOI en ambos PICs
    if(irq >= 8) {
        _outbyte(PIC2_CMD, PIC_EOI);
    }
    // Siempre enviar EOI al PIC maestro
    _outbyte(PIC1_CMD, PIC_EOI);
}