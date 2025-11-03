GLOBAL _divzero
GLOBAL _invalidopcode

SECTION .text

_divzero:
    push rbp
    mov rbp, rsp

    xor rcx, rcx
    xor rax, rax
    div rcx

    mov rsp, rbp
    pop rbp
    ret

_invalidopcode:
    push rbp
    mov rbp, rsp

    ud2

    mov rsp, rbp
    pop rbp
    ret