OUTPUT_FORMAT("binary")
ENTRY(loader) /* اسم رمز نقطة الدخول */

SECTIONS
{
    /* يجب وضع تعيين العنوان داخل كتلة SECTIONS */
    . = 0x08000000; /* العنوان الظاهري VMA = 3GB + 1MB */
    kernel_start = .;
    .text ALIGN(0x1000):
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
    data_end = .;

    .bss ALIGN(0x1000) :
    {
        *(COMMON)
        *(.bss .bss.*)
    }
    bss_end = .;

    kernel_end = .;
    kernel_size = kernel_end - kernel_start;
}
