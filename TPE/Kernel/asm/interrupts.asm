GLOBAL _cli
GLOBAL _sti
GLOBAL _hlt
GLOBAL haltcpu

GLOBAL picMasterMask
GLOBAL picSlaveMask

GLOBAL exception_divideByZero
GLOBAL exception_invalidOpCode

GLOBAL interrupt_timerTick
GLOBAL interrupt_keyboard
GLOBAL interrupt_syscall

GLOBAL _getSnapshot

EXTERN syscallHandler
EXTERN exceptionHandler
EXTERN getStackBase
EXTERN irqDispatcher ;estaba como irqHandler
EXTERN keyboard_check


;REVISAR ESTA PARTE 0x400000
section .rodata
    userland equ 0x400000 ;entry de la shell


SECTION .text

; CTX_SAVE keep_rax
;   keep_rax = 1 también guarda RAX
;   keep_rax = 0 NO guarda RAX
%macro CTX_SAVE 1
    %if %1
        push rax
    %endif
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro CTX_RESTORE 1
    pop  r15
    pop  r14
    pop  r13
    pop  r12
    pop  r11
    pop  r10
    pop  r9
    pop  r8
    pop  rsi
    pop  rdi
    pop  rbp
    pop  rdx
    pop  rcx
    pop  rbx
    %if %1
        pop  rax
    %endif
%endmacro

;=================================================================
; Layout de regs (20 qword) esperado por exceptions.c:
; 0:RAX 1:RBX 2:RCX 3:RDX 4:RSI 5:RDI 6:RBP 7:R8 8:R9 9:R10 10:R11
; 11:R12 12:R13 13:R14 14:R15 15:RIP 16:RSP 17:RFLAGS 18:CS 19:SS
;=================================================================
%macro fillSnapshot 0

    mov [regs + 0*8],  rax
    mov [regs + 1*8],  rbx
    mov [regs + 2*8],  rcx
    mov [regs + 3*8],  rdx
    mov [regs + 4*8],  rsi
    mov [regs + 5*8],  rdi
    mov [regs + 6*8],  rbp
    mov [regs + 7*8],  r8
    mov [regs + 8*8],  r9
    mov [regs + 9*8],  r10
    mov [regs +10*8],  r11
    mov [regs +11*8],  r12
    mov [regs +12*8],  r13
    mov [regs +13*8],  r14
    mov [regs +14*8],  r15

    ; ---- Marco que puso la CPU ----
    mov rax, [rsp + 0]         ; RIP
    mov [regs +15*8], rax

    mov rax, [rsp + 16]        ; RFLAGS
    mov [regs +17*8], rax

    mov rax, [rsp + 8]         ; CS
    mov [regs +18*8], rax

    ; Verificar si vino de userland para capturar RSP/SS correctamente
    mov rbx, [rsp + 8]         ; CS
    test bl, 3                 ; ¿CPL = 3? (userland)
    jz .from_kernel_snapshot   ; Si es 0, vino del kernel

    ; Desde userland: CPU pusheo SS y RSP
    mov rax, [rsp + 24]        ; RSP de usuario
    mov [regs +16*8], rax
    mov rax, [rsp + 32]        ; SS de usuario
    mov [regs +19*8], rax
    jmp .snapshot_done

.from_kernel_snapshot:
    ; Desde kernel: usar RSP actual (antes del push del marco)
    lea rax, [rsp + 40]        ; RSP antes del marco de interrupción (5 qwords)
    mov [regs +16*8], rax
    mov rax, 0x10              ; SS del kernel (típicamente segmento 0x10)
    mov [regs +19*8], rax

.snapshot_done:

%endmacro

;Devuelve &regs (lo usa dumpRegisters en exceptions.c)
_getSnapshot:
    mov rax, regs
    ret

;=======================
;    Exception Master
;=======================
; Parámetro: %1 = número de excepción (0=divide, 6=opcode)
;
; Comportamiento:
; - Si vino de USERLAND: muestra info, espera tecla, resetea a shell
; - Si vino de KERNEL:   muestra info (si es posible) y detiene sistema

%macro exceptionHandlerMacro 1
    ; Primero verificar origen ANTES de fillSnapshot (por si acaso)
    mov  rax, [rsp + 8]         ; CS del marco de interrupción
    test al, 3                  ; ¿CPL = 3? (userland)
    jz   .kernel_exception      ; Si es 0, vino del kernel -> panic

    ; === EXCEPCIÓN DESDE USERLAND ===
    fillSnapshot                ; Captura estado completo
    mov  rdi, %1                ; Vector de excepción (0 o 6)
    call exceptionHandler       ; Muestra pantalla azul, registros, espera tecla
    
    ; Al retornar de exceptionHandler, resetear a shell
    call getStackBase           ; RAX = RSP de usuario limpio
    mov  [rsp + 24], rax        ; RSP de retorno (user)
    mov  rax, userland          ; RIP de retorno (entry shell)
    mov  [rsp + 0],  rax
    iretq                       ; Retornar a shell con stack limpio

.kernel_exception:
    ; === EXCEPCIÓN DESDE KERNEL ===
    ; El kernel no debería fallar. Si falla, el sistema está corrupto.
    fillSnapshot                ; Captura lo que se pueda
    mov  rdi, %1
    
    ; Intentar mostrar info (puede fallar si el kernel está muy corrupto)
    call exceptionHandler
    
    ; Detener el sistema permanentemente
    cli                         ; Deshabilitar interrupciones
    hlt                         ; Detener CPU
    ; Por si acaso hlt retorna (NMI, SMI, etc):
.hang_forever:
    cli
    hlt
    jmp .hang_forever
%endmacro

_cli:
    cli  ; deshabilitar interrupciones
    ret

_sti:  
    sti  ; habilitar interrupciones
    ret

_hlt:  
    sti  ; dormir permitiendo despertar por IRQ
    hlt
    ret

haltcpu:
       cli
       hlt
       ret

;=================
;       PIC
;=================
%define PIC1_CMD   0x20
%define PIC1_DATA  0x21
%define PIC2_CMD   0xA0
%define PIC2_DATA  0xA1
%define PIC_EOI    0x20

picMasterMask:
    push rbp
    mov  rbp, rsp
    mov  ax, di
    out  PIC1_DATA, al
    pop  rbp
    retn

picSlaveMask:
    push rbp
    mov  rbp, rsp
    mov  ax, di
    out  PIC2_DATA, al
    pop  rbp
    retn

;==========================
;     Interrupt Master
;==========================
%macro irqHandlerMaster 1
	CTX_SAVE 1              ; Guardar todos los registros incluyendo RAX
	
	mov rdi, %1             ; Primer argumento: número de IRQ
	call irqDispatcher      ; Llamar handler en C (envía EOI internamente)
	
	CTX_RESTORE 1           ; Restaurar todos los registros incluyendo RAX
	iretq
%endmacro


;===========================
;   Exception handlers
;===========================

exception_divideByZero:            ;(#DE = 0)
    exceptionHandlerMacro 0


exception_invalidOpCode:            ;(#UD = 6)
    exceptionHandlerMacro 6

;===========================
;   Interrupts handlers
;===========================


interrupt_timerTick:    ;(IRQ0)
    irqHandlerMaster 0

interrupt_keyboard:     ;(IRQ1)
    irqHandlerMaster 1


interrupt_syscall:      ;(INT 0X80)
    CTX_SAVE 0                  ; NO guardar RAX
    mov  rdi, rax               ; rdi = número de syscall (SysV arg#1)
    call syscallHandler
    CTX_RESTORE 0               ; deja RAX con el valor de retorno
    iretq


SECTION .bss
regs resq 20 ;Para fillSnapshot

