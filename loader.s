global loader                   

MAGIC_NUMBER equ 0x1BADB002     
ALIGN_MODULES equ 0x00000001    
CHECKSUM equ -(MAGIC_NUMBER + ALIGN_MODULES)

section .text
align 4
    dd MAGIC_NUMBER
    dd ALIGN_MODULES
    dd CHECKSUM

loader:
    cli                         ; تعطيل المقاطعات فورا
    mov dx, 0x0430
    in eax, dx
    or eax, 3
    out dx, eax
    mov dx, 0xB2
    mov al, 0xA0
    out dx, al
    mov ax, 0x3C00
    mov dx, 0x0404
    out dx, ax
    ; المعالج الآن في العنوان العالي 0xC010xxxx بأمان تماماً
    
    ; 6. الآن فقط نقوم بنقل المعالج للمكدس العالي الدائم للكيرنل
    mov esp, boot_stack_top
    ; استدعاء دالة كيرنل المكتوبة بلغة C
    extern kmain
    push ebx
    call kmain                  

.loop:
    jmp .loop
section .bss
align 4                         
boot_stack_bottom:
    resb 16384                  ; رفع حجم المكدس لـ 16 كيلوبايت لتفادي الـ Overflow
boot_stack_top:
