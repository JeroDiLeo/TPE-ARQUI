include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin

# C sources in this directory (exclude files that start with _ )
SOURCES=$(wildcard [^_]*.c)

# Optional sources in subdirectories
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_LIB=$(wildcard lib/*.c)
SOURCES_SHELL=$(wildcard shell/*.c)
SOURCES_BENCHMARK=$(wildcard benchmark/*.c)

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_LIB=$(SOURCES_LIB:.c=.o)
OBJECTS_SHELL=$(SOURCES_SHELL:.c=.o)
OBJECTS_BENCHMARK=$(SOURCES_BENCHMARK:.c=.o)

LOADERSRC=_loader.c
LOADEROBJECT=$(LOADERSRC:.c=.o)

# If you have static libs, set STATILIBS to the linker inputs (e.g. libfoo.a)
STATILIBS=

all: $(MODULE)

# Link the final module into the parent Image/Userland folder
$(MODULE): $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_LIB) $(OBJECTS_SHELL) $(OBJECTS_BENCHMARK) $(STATILIBS)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld -o ../$(MODULE) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_LIB) $(OBJECTS_SHELL) $(OBJECTS_BENCHMARK) $(STATILIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./includes -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(GCC) $(GCCFLAGS) $(LOADERSRC) -c -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o lib/*.o shell/*.o benchmark/*.o *.o

.PHONY: all clean print