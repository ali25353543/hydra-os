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
    mov dx, 0x3F8
    in al, dx
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
