OUTPUT_FORMAT("binary")
ENTRY(entry)

SECTIONS
{
    . = 0x00110000; 

    .text : ALIGN(4)
    {
        *(.text.entry)     /* ملف الـ Assembly أولاً دائماً */
        *(.text)
        *(.text*)
    }

    .rodata : ALIGN(4)
    {
        *(.rodata*)        /* هنا تخزن نصوص الموديول مثل "hello" */
    }

    .data : ALIGN(4)
    {
        *(.data)
    }

    .bss : ALIGN(4)
    {
        *(.common)
        *(.bss)
    }
}
