db 'Hydra OS', 0
dd 0x40000000
db 'serial_init', 0
serial_init:
    mov ax, [esp + 8]
    mov bx, [esp + 4]
    mov cx, bx
    add ax, 3
    mov dx, ax
    mov ax, 0x80
    out dx, al
    ;;
    sub dx, 3
    shr cx, 8
    and cx, 0x00FF
    mov al, cl
    out dx, al
    ;;
    mov cx, bx
    and cx, 0x00FF
    mov al, cl
    out dx, al
    ;;
    add dx, 3
    mov al, 3
    out dx, al
    ;;
    sub dx, 1
    mov al, 0xC7
    out dx, al
    ;;
    add dx, 2
    mov al, 3
    out dx, al
    ;;
    mov ax, 0
    mov bx, 0
    mov cx, 0
    mov dx, 0
    ret

db 'serial_is_trasmit_fifo_empty', 0
serial_is_trasmit_fifo_empty:
    mov edx, [esp + 4]
    add edx, 5
    in ax, dx
    and eax, 0x20
    ret

db 'serial_write', 0
serial_write:
    mov esi, [esp + 4]
.loop:
    call serial_is_trasmit_fifo_empty
    cmp al, 0
    je .loop
    mov al, [esi]
    mov dx, 0x3F8
    out dx, al
    inc esi
    cmp al, 0
    jne serial_write
    mov ax, 0
    mov dx, 0
    mov esi, 0
    ret