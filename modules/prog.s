bits 32
org 0x01000000

jmp entry

entry:
    jmp entry

text db 'H', 0