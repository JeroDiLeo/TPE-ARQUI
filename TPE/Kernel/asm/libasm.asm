GLOBAL cpuVendor
GLOBAL _updateReg
GLOBAL _getRegs
GLOBAL _getKeyScan
GLOBAL _getSnapshot
GLOBAL exception_divideByZero
GLOBAL exception_invalidOpCode

extern exceptionHandler


section .text
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

_updateReg:
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
mov rax, [rsp]
mov [_registersMemory + 15*8], rax ;rip
lea rax, [rsp + 8] ; rsp antes del call (+8 para "deshacer" el push)
mov [_registersMemory + 16*8], rax ;rsp
ret

_getRegs:
mov rax, _registersMemory
ret

_getKeyScan:
xor rax, rax
in al, 0x64 ; hay que fijarnos el status del port, si esta en 0 no hay datos 
test al, 0x1
jnz .has_data
ret

.has_data:
in al, 0x60 ; posicion donde esta el data port del teclado en i/o
ret

; REVISAR TIENE ERRORES
; ========== EXCEPTION HANDLERS ==========
; Cuando ocurre una excepción, el CPU pushea automáticamente:
; [SS] [RSP] [RFLAGS] [CS] [RIP]
; Estos handlers guardan TODOS los registros (20 en total)

exception_divideByZero:
	call saveExceptionSnapshot
	mov rdi, 0  ; Exception type: ZERO_EXCEPTION
	call exceptionHandler
	call restoreExceptionSnapshot
	iretq

exception_invalidOpCode:
	call saveExceptionSnapshot
	mov rdi, 6  ; Exception type: INVALID_OPCODE_EXCEPTION
	call exceptionHandler
	call restoreExceptionSnapshot
	iretq

; Guarda el snapshot completo al momento de la excepción
saveExceptionSnapshot:
	; Guardar registros generales (0-14)
	mov [_snapshotMemory], rax
	mov [_snapshotMemory + 1*8], rbx
	mov [_snapshotMemory + 2*8], rcx
	mov [_snapshotMemory + 3*8], rdx
	mov [_snapshotMemory + 4*8], rsi
	mov [_snapshotMemory + 5*8], rdi
	mov [_snapshotMemory + 6*8], rbp
	mov [_snapshotMemory + 7*8], r8
	mov [_snapshotMemory + 8*8], r9
	mov [_snapshotMemory + 9*8], r10
	mov [_snapshotMemory + 10*8], r11
	mov [_snapshotMemory + 11*8], r12
	mov [_snapshotMemory + 12*8], r13
	mov [_snapshotMemory + 13*8], r14
	mov [_snapshotMemory + 14*8], r15
	
	; Los siguientes registros fueron pusheados por el CPU automáticamente
	; Stack: [return_addr] [RIP] [CS] [RFLAGS] [RSP] [SS]
	;         rsp+0        rsp+8  rsp+16 rsp+24  rsp+32 rsp+40
	
	mov rax, [rsp + 8]              ; RSP antes de la excepción
	mov [_snapshotMemory + 15*8], rax
	
	mov rax, [rsp + 16]             ; RIP (instruction pointer que causó la excepción)
	mov [_snapshotMemory + 16*8], rax
	
	mov rax, [rsp + 24]             ; RFLAGS
	mov [_snapshotMemory + 17*8], rax
	
	mov rax, [rsp + 32]             ; CS (Code Segment)
	mov [_snapshotMemory + 18*8], rax
	
	mov rax, [rsp + 40]             ; SS (Stack Segment)
	mov [_snapshotMemory + 19*8], rax
	
	ret

; Restaura los registros desde el snapshot
restoreExceptionSnapshot:
	mov rax, [_snapshotMemory]
	mov rbx, [_snapshotMemory + 1*8]
	mov rcx, [_snapshotMemory + 2*8]
	mov rdx, [_snapshotMemory + 3*8]
	mov rsi, [_snapshotMemory + 4*8]
	mov rdi, [_snapshotMemory + 5*8]
	mov rbp, [_snapshotMemory + 6*8]
	mov r8, [_snapshotMemory + 7*8]
	mov r9, [_snapshotMemory + 8*8]
	mov r10, [_snapshotMemory + 9*8]
	mov r11, [_snapshotMemory + 10*8]
	mov r12, [_snapshotMemory + 11*8]
	mov r13, [_snapshotMemory + 12*8]
	mov r14, [_snapshotMemory + 13*8]
	mov r15, [_snapshotMemory + 14*8]
	ret

; Retorna un puntero al snapshot de la excepción
_getSnapshot:
	mov rax, _snapshotMemory
	ret


; reserva para mandar copia de los registros 
section .bss
_registersMemory resq 17
;REVISAR
_snapshotMemory resq 20  ; 20 registros para excepciones (incluye RFLAGS, CS, SS) 