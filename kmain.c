#include "multiboot.h"
#include "serial.h"
#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "shell.h"
#include "io.h"
#include "fb.h"
#include "ata.h"
#include "fat32.h"

void kmain(unsigned int ebx)
{
    serial_configure_baud_rate(SERIAL_COM1_BASE, 3);
    serial_configure_fifo_buffer(SERIAL_COM1_BASE);
    serial_configure_line(SERIAL_COM1_BASE);
    serial_configure_modem(SERIAL_COM1_BASE);
    serial_is_transmit_fifo_empty(SERIAL_COM1_BASE);
    serial_write("kernel starting...\n");

    multiboot_info_t *mbi = (multiboot_info_t *)ebx;
    gdt_install();
    serial_write("GDT installed!\n");

    idt_install();
    serial_write("IDT installed!\n");
    
    keyboard_init();
    serial_write("keyboard initlazed!\n");

    __asm__("sti");
    serial_write("Interrupts Enabled!\n");
        unsigned int addr = mbi->mods_addr;
        typedef void (*call_module_t)(void);
        call_module_t start = (call_module_t) addr;
        start();
    shell_init();

    ata_identify();
    while (1)
    {
        shell_update();
    }   
}