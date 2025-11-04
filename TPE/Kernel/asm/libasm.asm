GLOBAL cpuVendor
GLOBAL _updateReg
GLOBAL _getRegs
GLOBAL _getKeyScan
GLOBAL _getSnapshot
GLOBAL exception_divideByZero
GLOBAL exception_invalidOpCode
GLOBAL _inbyte
GLOBAL _outbyte
GLOBAL _readTime

extern exceptionHandler
extern irqLastStackPtr


section .text
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	xor eax, eax
	cpuid

	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx
	mov byte [rdi + 12], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

_updateReg:
	 mov rdx, [rel irqLastStackPtr]
        test rdx, rdx
        jz .done

        ; RAX..R15 (orden esperado por userland)
        mov rax, [rdx + 112]
        mov [_registersMemory + 0*8], rax

        mov rax, [rdx + 104]
        mov [_registersMemory + 1*8], rax

        mov rax, [rdx + 96]
        mov [_registersMemory + 2*8], rax

        mov rax, [rdx + 88]
        mov [_registersMemory + 3*8], rax

        mov rax, [rdx + 64]
        mov [_registersMemory + 4*8], rax

        mov rax, [rdx + 72]
        mov [_registersMemory + 5*8], rax

        mov rax, [rdx + 80]
        mov [_registersMemory + 6*8], rax

        mov rax, [rdx + 56]
        mov [_registersMemory + 7*8], rax

        mov rax, [rdx + 48]
        mov [_registersMemory + 8*8], rax

        mov rax, [rdx + 40]
        mov [_registersMemory + 9*8], rax

        mov rax, [rdx + 32]
        mov [_registersMemory + 10*8], rax

        mov rax, [rdx + 24]
        mov [_registersMemory + 11*8], rax

        mov rax, [rdx + 16]
        mov [_registersMemory + 12*8], rax

        mov rax, [rdx + 8]
        mov [_registersMemory + 13*8], rax

        mov rax, [rdx + 0]
        mov [_registersMemory + 14*8], rax

        ; RIP
        mov rax, [rdx + 120]
        mov [_registersMemory + 15*8], rax

        ;
        ; --- Marco de interrupción y reconstrucción de RSP --------------------
        ; El CTX_SAVE deja en memoria, a partir de rdx, el frame con los registros
        ; generales (R15..RAX) seguido del trío que apila el hardware: RIP, CS y
        ; RFLAGS.  Si la interrupción ocurrió mientras estábamos en CPL3, la CPU
        ; también empuja el RSP y el SS del proceso antes de saltar al stack de
        ; kernel. En cambio, cuando la interrupción se atiende desde CPL0, no
        ; hay cambio de stack: el RSP original es simplemente la dirección
        ; inmediatamente posterior al frame salvado por CTX_SAVE.
        ;
        ; Para distinguir ambos escenarios, usamos los bits 0-1 del selector de
        ; código: contienen el CPL con el que se disparó la interrupción.  Un CPL
        ; distinto de cero implica un cruce de usuario->kernel y, por ende, que la
        ; CPU apiló un RSP explícito; CPL=0 indica que debemos reconstruirlo de
        ; forma manual.
        ; ----------------------------------------------------------------------

        ; RSP depende del CPL al momento de la interrupción
        mov ecx, [rdx + 128]     ; Selector CS guardado (los bits 0-1 almacenan el CPL)
        test cl, 3               ; CPL = 0? (interrupción en modo kernel)
        jz .from_kernel

        mov rax, [rdx + 144]     ; RSP apilado por la CPU (cambio de privilegio)
        jmp .store_rsp

.from_kernel:
        lea rax, [rdx + 144]     ; RSP original = dirección posterior al frame

.store_rsp:
        mov [_registersMemory + 16*8], rax

.done:
        ret

_getRegs:
	mov rax, _registersMemory
	ret

_getKeyScan:
	xor rax, rax
	in al, 0x64              ; Leer status del teclado
	test al, 0x1             ; Verificar si hay datos disponibles
	jnz .has_data
	ret

.has_data:
	in al, 0x60              ;Posicion donde esta el data port del teclado en i/o
	ret

; es void esta funcion
_outbyte:
	push rdx; guardo lo que tenia rdx antes
	mov dx, di
	mov al, sil
	out dx, al
	pop rdx
	ret

; devuelve 8 bits(uint8_t )
_inbyte:
	push rdx; guardo lo que tenia rdx antes
	mov dx, di
	in al, dx
	pop rdx; recupero lo que tenia rdx antes
	ret

_readTime:
	push rdx
	
	; Seleccionar registro del RTC
	mov dx, 0x70
	mov al, dil
	or al, 0x80              ; Deshabilitar NMI durante la lectura
	out dx, al
	
	; Delay para que el hardware del RTC procese
	jmp short $+2
	jmp short $+2            ; Doble delay para mayor seguridad
	
	; Leer valor del RTC
	xor eax, eax
	mov dx, 0x71
	in al, dx
	
	pop rdx
	ret

; reserva para mandar copia de los registros 
section .bss
_registersMemory resq 17
