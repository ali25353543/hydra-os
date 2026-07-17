global start
extern main
section .text
    start:
        push ebp
        mov esp, ebp
        call main
        pop ebp
        mov ebp, esp
        ret