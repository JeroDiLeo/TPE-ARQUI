#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include "interrupts.h"
#include "idtLoader.h"


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
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
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
    ncPrint("[Initializing system]");
    ncNewline();
    
    // Load and configure IDT
    ncPrint("  Loading IDT... ");
    load_idt();
    ncPrint("Done");
    ncNewline();
    
    ncPrint("[Kernel Main]");
    ncNewline();
    
    // Verify module is loaded correctly
    ncPrint("  Sample code module at 0x");
    ncPrintHex((uint64_t)sampleCodeModuleAddress);
    ncNewline();
    
    // DEBUG: Show first bytes of module
    ncPrint("  First 32 bytes of module:");
    ncNewline();
    uint8_t *bytes = (uint8_t*)sampleCodeModuleAddress;
    for(int i = 0; i < 32; i++) {
        if(i % 16 == 0) ncPrint("    ");
        ncPrintHex(bytes[i]);
        ncPrint(" ");
        if(i % 16 == 15) ncNewline();
    }
    ncNewline();
    
    // Check if module appears to be valid
    ncPrint("  Checking sample code module... ");
    if (*(uint64_t*)sampleCodeModuleAddress == 0) {
        ncPrint("ERROR: Module appears to be empty");
        ncNewline();
        while(1) {
            _hlt();
        }
    }
    ncPrint("OK");
    ncNewline();
    
    ncPrint("  Sample data module at 0x");
    ncPrintHex((uint64_t)sampleDataModuleAddress);
    ncNewline();
    ncPrint("  Sample data module contents: ");
    ncPrint((char*)sampleDataModuleAddress);
    ncNewline();
    ncNewline();

    ncPrint("[About to transfer control...]");
    ncNewline();
    
    // Enable interrupts before entering userland
    _sti();
    
    ncPrint("[Transferring control to userland]");
    ncNewline();
    ncNewline();
    
    // Execute module - should never return
    ((EntryPoint)sampleCodeModuleAddress)();
    
    // If we get here, the module returned unexpectedly
    ncPrint("[Module exited - halting system]");
    ncNewline();
    
    while(1) {
        _hlt();
    }
    
    return 0;
}
