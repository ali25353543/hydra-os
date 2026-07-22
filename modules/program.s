global entry
section .text.entry
    extern module
    entry:
        push dword [esp + 8]
        push dword [esp + 8]

        call module
        add esp, 8
        ret