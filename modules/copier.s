org 0x01000000
bits 32

copier:
    push ebx
    push esi
    mov eax, [esp + 12] ;Start Address
    mov ebx, [esp + 8] ;End Address
    mov ecx, [esp + 4] ;Jump Address
    mov edx, eax
    sub edx, ebx
    mov esi, ecx
    mov ecx, edx
    mov edi, esi
    jmp main
main:
    mov bl, [eax]
    mov [edi], bl
    inc eax
    inc edi
    loop main
    jmp esi
    pop esi
    pop ebx
    mov eax, 0
    ret