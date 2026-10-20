[BITS 32]
global enter_real_mode
org 0x600 ; ← احذف org 0x600 إذا كان في kernel

db 1 ;Real-mode Program

section .text
enter_real_mode:
    cli
    
    ; حفظ الحالة
    mov [saved_esp], esp
    mov [saved_ss], ss
    mov [saved_ds], ds
    mov [saved_es], es
    mov [saved_fs], fs
    mov [saved_gs], gs
    sgdt [saved_gdt]
    sidt [saved_idt]
    
    ; عطّل Paging
    mov eax, cr0
    and eax, 0x7FFFFFFF
    mov cr0, eax
    
    ; حمّل GDT المؤقتة
    lgdt [gp]
    
    ; حمّل 16-bit data segments
    mov ax, 0x20
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; far jump → 16-bit code
    jmp 0x18:pm16_entry

[BITS 16]
pm16_entry:
    ; عطّل PE
    mov eax, cr0
    and eax, 0xFFFFFFFE
    mov cr0, eax
    ; far jump → Real Mode
    jmp 0x0000:real_mode_entry

real_mode_entry:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov sp, 0x7C00
    lidt [idt_real]
    
    ; ★★★ شغّل البرنامج (مقاطعات معطّلة) ★★★
    sti
main_real:
    ;mov cx, 0x0001
    ;mov dx, 0x0080
    ;mov ax, 0x7C0
    ;mov es, ax
    ;mov bx, 0
    ;mov ax, 0x0201
    ;int 0x13
    
    ; ★★★ العودة إلى PM ★★★
    cli
    lgdt [gp]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:pm32_return

[BITS 32]
pm32_return:
    ; استعد GDT/IDT الأصلية
    lgdt [saved_gdt]
    lidt [saved_idt]
    
    ; حمّل segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; استعد ESP
    mov esp, [saved_esp]
    
    ; ★★★ العودة إلى kernel ★★★
    sti
    ret

; ============================================
; GDT المؤقتة (5 entries!)
; ============================================
align 8
gdt:
    dq 0                                    ; 0x00: null
    dq 0x00CF9A000000FFFF                   ; 0x08: 32-bit code
    dq 0x00CF92000000FFFF                   ; 0x10: 32-bit data
    dq 0x00009A000000FFFF                   ; 0x18: 16-bit code ★
    dq 0x000092000000FFFF                   ; 0x20: 16-bit data ★
gdt_end:

gp:
    dw gdt_end - gdt - 1                    ; limit = 39 (5*8 - 1)
    dd gdt

idt_real:
    dw 0x3FF
    dd 0x0000

saved_esp:  dd 0
saved_ss:   dw 0
saved_ds:   dw 0
saved_es:   dw 0
saved_fs:   dw 0
saved_gs:   dw 0
saved_gdt:  dw 0
            dd 0
saved_idt:  dw 0
            dd 0