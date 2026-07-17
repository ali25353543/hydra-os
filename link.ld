ENTRY(loader) /* اسم رمز نقطة الدخول */

SECTIONS
{
    /* يجب وضع تعيين العنوان داخل كتلة SECTIONS */
    . = 0x00100000; /* العنوان الظاهري VMA = 3GB + 1MB */
    kernel_phisycal_address = .;
    .text ALIGN(0x1000) :
    {
        *(.text .text.*) /* جميع أجزاء الكود */
    }

    .rodata ALIGN(0x1000) :
    {
        *(.rodata*) /* البيانات القابلة للقراءة فقط */
    }

    .data ALIGN(0x1000) :
    {
        *(.data .data.*) /* البيانات المعرفة المتغيرة */
    }

    .bss ALIGN(0x1000) :
    {
        *(COMMON)
        *(.bss .bss.*)
    }
    kernel_phisycal_end = .;
}
