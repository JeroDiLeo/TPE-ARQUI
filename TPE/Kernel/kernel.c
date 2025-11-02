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
	//dsp sacar 
	ncPrint("\n  DEBUG: About to jump to 0x");
    ncPrintHex((uint64_t)sampleCodeModuleAddress);
    ncPrint("\n  DEBUG: First bytes of module: ");
    uint8_t *moduleBytes = (uint8_t*)sampleCodeModuleAddress;
    for(int i = 0; i < 16; i++){
        ncPrintHex(moduleBytes[i]);
        ncPrint(" ");
    }
    ncNewline();
	//dsp sacar
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
	
	return 0;
}
