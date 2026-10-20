bits 32
org 0x00010000
db 0 ;Normal Program

main:
    mov word [0xB8000], 0x0F41
    jmp main

text db 'H', 0
