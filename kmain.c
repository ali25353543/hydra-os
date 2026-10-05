#include <multiboot.h>
#include <serial.h>
#include <gdt.h>
#include <idt.h>
#include <keyboard.h>
#include <shell.h>
#include <io.h>
#include <fb.h>
#include <ata.h>
#include <fat32.h>
#include <users.h>
#include <tar.h>
#include <vars.h>

void kmain(unsigned int ebx)
{
    multiboot_info_t *mbi = (multiboot_info_t *)ebx;
    module_t *mod = (module_t *)mbi->mods_addr;
    tar_init((char *)mod[0].mod_start);
    serial_configure_baud_rate(SERIAL_COM1_BASE, 12);
    serial_configure_fifo_buffer(SERIAL_COM1_BASE);
    serial_configure_line(SERIAL_COM1_BASE);
    serial_configure_modem(SERIAL_COM1_BASE);
    serial_is_transmit_fifo_empty(SERIAL_COM1_BASE);
    //exec("serial.bin", "--divisor 12");
    
    gdt_install();
    serial_write("GDT installed!\n");

    idt_install();
    serial_write("IDT installed!\n");
    
    keyboard_init();
    serial_write("keyboard initlazed!\n");

    __asm__("sti");
    serial_write("Interrupts Enabled!\n");

    //ata_init();
    //serial_write("ATA installed!\n");
        //unsigned int module_start_address = mod[0].mod_start;
        
        serial_write("Jumping to module...\n");
        
        // 2. تحديث الـ typedef ليقبل مؤشرات الدوال كمعاملات (Arguments)
        //typedef void (*call_module_t)(void);
        
        // 3. تحويل العنوان إلى مؤشر الدالة الجديد
        //call_module_t start = (call_module_t) module_start_address;
        
        // 4. الاستدعاء الآن سيمر بدون أخطاء تجميع
        //start;

    // تأكد من أن السلسلة صحيحة
 // تهيئة المستخدمين
    
    void vga_load_font(uint8_t *font_matrix);
    users_init(tar_read("users.txt"));
    char font_data[4096]; // Assuming a 256-character font with 16 bytes each
    char *font_matrix = tar_read("en.psf");
    font_matrix += 4;
    for (int i = 0; i < 4096; i++)
    {
        font_data[i] = font_matrix[i];
    }
    vga_load_font((unsigned char *)font_data);
    fb_putc('\n');
    void fb_switch_page(unsigned char page_num);
    //fb_switch_page(1);
    shell_init(login(), 1);
    //fb_puts((char *)ata_read_sector(0));
    while (1)
    {
        shell_update();
    }
}
