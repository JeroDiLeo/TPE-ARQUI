#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>

// Prototipos de syscalls (implementadas en asm/syscall.asm)
extern uint64_t _sys_read(uint64_t fd, char *buffer, uint64_t count);
extern uint64_t _sys_write(uint64_t fd, const char *buffer, uint64_t count, uint64_t color);
extern uint64_t _sys_drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t width, uint64_t height);
extern uint64_t _sys_clearScreen(void);
extern uint64_t _sys_getCoords(void);
extern uint64_t _sys_getScreenInfo(void);
extern uint64_t _sys_getFontInfo(void);
extern uint64_t _sys_getTime(uint64_t component);
extern uint64_t _sys_setFontScale(uint64_t scale);
extern uint64_t _sys_getRegisters(uint64_t *buffer);
extern uint64_t _sys_sleep(uint64_t milliseconds);
extern uint64_t _sys_playSound(uint64_t frequency, uint64_t duration);
extern void _sys_exit(int exitCode) __attribute__((noreturn));
extern uint64_t _sys_waitKey(void);
extern uint64_t _sys_getTicks(void);
 
// Wrappers con nombres más amigables
#define sys_read(fd, buf, count) _sys_read(fd, buf, count)
#define sys_write(fd, buf, count, color) _sys_write(fd, buf, count, color)
#define sys_draw_rectangle(Hexcolor, x, y, w, h) _sys_drawRectangle(Hexcolor, x, y, w, h)
#define sys_clear_screen() _sys_clearScreen()
#define sys_get_coords() _sys_getCoords()
#define sys_get_screen_info() _sys_getScreenInfo()
#define sys_get_font_info() _sys_getFontInfo()
#define sys_get_time(component) _sys_getTime(component)
#define sys_set_font_scale(scale) _sys_setFontScale(scale)
#define sys_get_registers(buffer) _sys_getRegisters(buffer)
#define sys_sleep(ms) _sys_sleep(ms)
#define sys_play_sound(freq, duration) _sys_playSound(freq, duration)
#define sys_exit(code) _sys_exit(code)
// Bloquea hasta recibir una tecla desde el teclado (IRQ1)
#define sys_wait_key() _sys_waitKey()
#define sys_get_ticks() _sys_getTicks()



#endif
