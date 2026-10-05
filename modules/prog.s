bits 32
org 0x01000000

push ebx
push esi
mov ebx, 0xB8000

jmp entry

entry:
    mov dx, 0x3F8
    in al, dx
    cmp al, 0
    je .skip
    mov [ebx], al
    mov byte [ebx + 1], 0x0F
    add ebx, 2
    jmp entry
.skip:
    add ebx, 2
    jmp entry

text db 'H', 0
