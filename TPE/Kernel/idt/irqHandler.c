#include "../include/time.h"
#include "../include/KBDriver.h"

void irqDispatcher(uint64_t irq, uint64_t * registers) {
  switch (irq) {
    case 0: int_20h();
    break;
    case 1: int_21h();
    break;

  }
}

static void int_20h() {
    timer_handler();
}

static void int_21h() {
    keyboard_check();
}
