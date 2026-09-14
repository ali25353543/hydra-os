bits 32
org 0x01000000

jmp entry

entry:
    mov eax, 1
    mov ebx, 0
    int 0x80

text db 'H', 0