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
	; Guardar todos los registros de propósito general
	mov [_registersMemory], rax
	mov [_registersMemory + 1*8], rbx
	mov [_registersMemory + 2*8], rcx
	mov [_registersMemory + 3*8], rdx
	mov [_registersMemory + 4*8], rsi
	mov [_registersMemory + 5*8], rdi
	mov [_registersMemory + 6*8], rbp
	mov [_registersMemory + 7*8], r8
	mov [_registersMemory + 8*8], r9
	mov [_registersMemory + 9*8], r10
	mov [_registersMemory + 10*8], r11
	mov [_registersMemory + 11*8], r12
	mov [_registersMemory + 12*8], r13
	mov [_registersMemory + 13*8], r14
	mov [_registersMemory + 14*8], r15
	
	; Guardar RIP (dirección de retorno en el stack)
	mov rax, [rsp]
	mov [_registersMemory + 15*8], rax
	
	; Guardar RSP antes del call (+8 para compensar el push del call)
	lea rax, [rsp + 8]
	mov [_registersMemory + 16*8], rax
	
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
