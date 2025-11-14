global loader
extern kmain                  ; tell assembler that we’ll call a C function named kmain

MAGIC_NUMBER equ 0x1BADB002
FLAGS equ 0x0
CHECKSUM equ -MAGIC_NUMBER

section .text
align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM

KERNEL_STACK_SIZE equ 4096     ; 4 KB stack

section .bss
align 4
kernel_stack:
    resb KERNEL_STACK_SIZE     ; reserve 4 KB of uninitialized stack memory


loader:
    ; --- setup stack ---
    mov esp, kernel_stack + KERNEL_STACK_SIZE
    ; now esp points to top of our reserved stack

    ; --- call C code ---
    call kmain                ; jump into your C kernel function

.hang:
    jmp .hang                 ; loop forever (in case kmain returns)
