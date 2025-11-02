/* _loader.c */
#include <stdint.h>

extern char bss;
extern char endOfBinary;

int main();

void * memset(void * destiny, int32_t c, uint64_t length);

void _start() {
	// Clean BSS
	memset(&bss, 0, &endOfBinary - &bss);

	// Call main - it should never return
	main();
	
	// If main returns, halt forever
	while(1) {
		__asm__ __volatile__("hlt");
	}
}


void * memset(void * destiation, int32_t c, uint64_t length) {
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destiation;

	while(length--)
		dst[length] = chr;

	return destiation;
}
