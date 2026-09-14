[BITS 32]
org 0x00100000

global _start
_start:
    ; حفظ مسجلات النواة لحمايتها
    push ebx
    push esi
    mov word [0xB8000], 0X410F

    ; 1. جلب مؤشر الـ argv من المكدس
    ; المكدس يحتوي على: [esp + 12] وهو مؤشر مصفوفة args الممررة من دالة exec
    mov edx, [esp + 12] 
    
    ; 2. استخراج المتغيرات من المصفوفة args
    mov eax, [edx + 4]   ; جلب الـ Divisor (الذي قيمته 12)
    mov ebx, [edx]       ; جلب رقم المنفذ Base Port (الذي قيمته 0x3F8)

    ; 3. تهيئة المنفذ (نفس كود التهيئة الخاص بك)
    mov ecx, ebx
    add ax, 3
    mov dx, ax
    mov ax, 0x80
    out dx, al
    sub dx, 3
    shr cx, 8
    and cx, 0x00FF
    mov al, cl
    out dx, al
    mov cx, bx
    and cx, 0x00FF
    mov al, cl
    out dx, al
    add dx, 3
    mov al, 3
    out dx, al
    sub dx, 1
    mov al, 0xC7
    out dx, al
    add dx, 2
    mov al, 3
    out dx, al

    ; 4. الآن نقوم بطباعة رسالة اختبارية مباشرة من داخل الموديول للتأكد من أنه يعمل!
    mov esi, test_msg
.print_loop:
    mov al, [esi]
    cmp al, 0
    je .done
    mov dx, 0x3F8
    out dx, al           ; إرسال بايت مباشرة بدون حلقة الفحص للتأكد
    inc esi
    jmp .print_loop

.done:
    ; استعادة مسجلات النواة بأمان والعودة لـ kmain
    pop esi
    pop ebx
    xor eax, eax
    ret

; نص اختباري نضعه في نهاية الملف تماماً بعيداً عن أعين المعالج عند الدخول
test_msg: db " [Hello from Serial Module!] ", 0
