bits 32
org 0x01000000
jmp 0x18:pm_16

pm_16:
    push esi
    mov eax, cr0
    and eax, 0xFFFFFFFE
    mov cr0, eax
    jmp 0x7C00
    ret