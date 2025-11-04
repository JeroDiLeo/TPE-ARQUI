#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include "interrupts.h"
#include "idtLoader.h"
#include "SysCall_Lib.h"


extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				
		- sizeof(uint64_t)			
	);
}

void * initializeKernelBinary()
{
	char buffer[10];

	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	return getStackBase();
}

int main()
{
    // Clear screen at startup
    ncClear();
    
    ncPrint("[Initializing system]");
    ncNewline();
    
    // Load and configure IDT
    ncPrint("  Loading IDT... ");
    load_idt();
    ncPrint("Done");
    ncNewline();
    
    // Enable interrupts explicitly
    ncPrint("  Enabling interrupts... ");
    _sti();
    ncPrint("Done");
    ncNewline();
    ncNewline();
    
    ncPrint("[Kernel Main]");
    ncNewline();
    
    // Detailed module verification
    ncPrint("  Checking module at 0x");
    ncPrintHex((uint64_t)sampleCodeModuleAddress);
    ncNewline();
    
    // Display first few bytes of the module
    ncPrint("  Module header bytes: ");
    unsigned char *moduleStart = (unsigned char *)sampleCodeModuleAddress;
    for(int i = 0; i < 8; i++) {
        ncPrintHex(moduleStart[i]);
        ncPrint(" ");
    }
    ncNewline();
	ncPrint("  Sample code module at 0x");
	ncPrintHex((uint64_t)sampleCodeModuleAddress);
	ncNewline();
	
	// Check if module appears to be valid
	ncPrint("  Checking sample code module... ");
	if (*(uint64_t*)sampleCodeModuleAddress == 0) {
		ncPrint("ERROR: Module appears to be empty\n");
		return -1;
	}
	ncPrint("OK");
	ncNewline();
	
	// Execute module
	ncPrint("  Calling the sample code module returned: ");
	int result = ((EntryPoint)sampleCodeModuleAddress)();
	ncPrintHex(result);
	ncNewline();
	ncNewline();

	ncPrint("  Sample data module at 0x");
	ncPrintHex((uint64_t)sampleDataModuleAddress);
	ncNewline();
	ncPrint("  Sample data module contents: ");
	ncPrint((char*)sampleDataModuleAddress);
	ncNewline();
	

	ncPrint("[Finished]");
	ncNewline();
	
	// Verificar si el módulo pidió salir
	if (checkShouldExit()) {
		ncPrint("Exit requested with code: ");
		ncPrintHex(getLastExitCode());
		ncNewline();
		ncPrint("Shutting down...");
		ncNewline();
		
		// Apagar QEMU usando el puerto especial de QEMU
		// Esto funciona en QEMU y hace que termine la emulación
		__asm__ volatile (
			"mov $0x2000, %%rax\n"
			"mov $0x604, %%rdx\n"
			"out %%ax, %%dx\n"
			:
			:
			: "rax", "rdx"
		);
	}
	
	// Si no hay exit, loop infinito
	while(1) {
		__asm__ volatile ("hlt");
	}
	
	return 0;
}
