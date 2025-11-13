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
GLOBAL interrupt_exit

GLOBAL _getSnapshot
GLOBAL irqLastStackPtr

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
    jz %%from_kernel_snapshot   ; Si es 0, vino del kernel (iba $)

    ; Desde userland: CPU pusheo SS y RSP
    mov rax, [rsp + 24]        ; RSP de usuario
    mov [regs +16*8], rax
    mov rax, [rsp + 32]        ; SS de usuario
    mov [regs +19*8], rax
    jmp %%snapshot_done

%%from_kernel_snapshot:
    ; Desde kernel: recuperar RSP original (CPU solo pushea RIP, CS y RFLAGS)
    lea rax, [rsp + 24]        ; RSP previo a la interrupción (3 qwords)
    mov [regs +16*8], rax
    ; Recuperar el selector de segmento real del kernel
    mov ax, ss
    movzx eax, ax
    mov [regs +19*8], rax

%%snapshot_done:

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
    ; Detectar si la excepción proviene de userland o kernel
    ; 1) Si CPL=3 => seguro userland (stack switch: frame incluye SS:RSP)
    ; 2) Si CPL!=3, pero RIP pertenece [0x400000,0x600000) => userland en CPL0
    ; 3) Caso contrario => kernel

    mov  rbx, [rsp + 0]         ; RIP
    mov  rax, [rsp + 8]         ; CS
    mov  rcx, rax
    and  ecx, 3
    cmp  ecx, 3
    je   %%from_user_ring3

    ; Comprobar rango de módulo de usuario cargado (0x400000..0x600000)
    mov  rdx, rbx
    cmp  rdx, 0x400000
    jb   %%from_kernel
    cmp  rdx, 0x600000
    jae  %%from_kernel
    jmp  %%from_user_ring0

%%from_user_ring3:
    ; === USERLAND (CPL3) ===
    fillSnapshot
    mov  rdi, %1
    call exceptionHandler
    call getStackBase           ; RAX = nuevo RSP de usuario
    mov  [rsp + 24], rax        ; En CPL3 el frame incluye SS:RSP
    mov  rax, userland          ; nuevo RIP
    mov  [rsp + 0],  rax
    iretq

%%from_user_ring0:
    ; === USERLAND en CPL0 (sin cambio de pila) ===
    fillSnapshot
    mov  rdi, %1
    call exceptionHandler
    
    ; Resetear stack manualmente (no hay SS:RSP en el frame)
    call getStackBase
    mov  rsp, rax               ; Resetear RSP directamente
    sub  rsp, 24                ; Espacio para frame de iretq (RIP, CS, RFLAGS)
    
    ; Reconstruir frame de interrupción
    mov  rax, userland
    mov  [rsp + 0], rax         ; RIP = 0x400000
    mov  word [rsp + 8], 0x08   ; CS = kernel code segment
    pushfq
    pop  qword [rsp + 16]       ; RFLAGS = actual
    
    iretq

%%from_kernel:
    ; === KERNEL ===
    fillSnapshot
    mov  rdi, %1
    call exceptionHandler
    cli
    hlt
%%hang_forever:
    cli
    hlt
    jmp %%hang_forever
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
        mov [rel irqLastStackPtr], rsp

        mov rdi, %1             ; Primer argumento: número de IRQ
        call irqDispatcher      ; Llamar handler en C (envía EOI internamente)

        mov qword [irqLastStackPtr], 0

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
    mov  r9, rax                ; pasar número de syscall como 6to argumento (SysV usa R9)
    call syscallHandler
    CTX_RESTORE 0               ; deja RAX con el valor de retorno
    iretq

; ===========================
;           EXIT 
; ===========================

interrupt_exit:         ;(INT 0X81) - Terminar proceso y volver a shell
    call getStackBase           ; RAX = RSP de usuario limpio
    mov  [rsp + 24], rax        ; RSP de retorno (posición en el marco de interrupción)
    mov  rax, userland          ; RIP de retorno (entry shell en 0x400000)
    mov  [rsp + 0],  rax        ; Sobrescribir RIP en el marco
    iretq                       ; Retornar a shell con stack limpio


SECTION .bss
regs resq 20 ;Para fillSnapshot
irqLastStackPtr resq 1