#include <ata.h>
#include <serial.h>
#include <fb.h>
static unsigned short sector[256];

void ata_init() {
    outb(0x3F6, 0x04); // تفعيل الـ Reset
// تأخير بسيط (يمكنك استخدام قراءة السجل كمؤقت)
    inb(0x3F6); inb(0x3F6); inb(0x3F6); inb(0x3F6); 
    outb(0x3F6, 0x00); // إيقاف الـ Reset ليعود المتحكم للعمل
// انتظر حتى يختفي بت Busy بعد إعادة التشغيل
    while ((inb(0x1F7) & 0x80) != 0);
}

void ata_identify() {
    ata_init();
// --- 2. إرسال أمر IDENTIFY ---
    outb(0x1F6, 0xA0); // اختيار Master
    outb(0x1F1, 0x00); // تصفير سجل الميزات (مهم)
    outb(0x1F2, 0);    
    outb(0x1F3, 0);    
    outb(0x1F4, 0);    
    outb(0x1F5, 0);    
    outb(0x1F7, 0xEC); // أمر IDENTIFY

    // Polling
    while ((inb(0x1F7) & 0x80) != 0);
    while ((inb(0x1F7) & 0x08) == 0);

    int i;
    for (i = 0; i < 256; i++) {
        sector[i] = inw(0x1F0);
    }
    unsigned a = inb(ATA_PIO_LBA_MID_PORT);
    unsigned b = inb(ATA_PIO_LBA_HI_PORT);
    if (!a && !b) {
        serial_write("PATA\n");
    }
    if (unix) {
        serial_write("You are in UNIX-like OS\n");
    }
    // طباعة نوع العنونة
    if (sector[49] & (1 << 9)) {outb(0x3F6, 0x04); // تفعيل الـ Reset
// تأخير بسيط (يمكنك استخدام قراءة السجل كمؤقت)
    inb(0x3F6); inb(0x3F6); inb(0x3F6); inb(0x3F6); 
    outb(0x3F6, 0x00); // إيقاف الـ Reset ليعود المتحكم للعمل
        serial_write("LBA Addressing: ");
        if (sector[83] & (1 << 10)) {
            serial_write("48-bit\n");
        } else {
            serial_write("28-bit\n");
        }
    } else {
    serial_write("CHS Addressing Only\n");
    }
}

unsigned short *ata_read_sector(unsigned long long sector_num) {
    // 1. الانتظار حتى يصبح القرص غير مشغول (BSY == 0)
    while ((inb(0x1F7) & 0x80) != 0);
    ata_init();
    // 2. إرسال سجل المحرك (Drive Select)
    // في نمط LBA 48، نرسل القيمة 0x40 فقط في البت السادس لتفعيل LBA والـ Master
    // (لا ندمج أي بتات من عنوان القطاع في هذا المنفذ كما كنا نفعل في LBA 28)
    outb(0x1F6, 0x40); 

    // 3. إرسال عدد القطاعات (Sector Count) - وهو 16 بت في LBA 48
    // نرسل البايت العالي (High Byte) ثم البايت المنخفض (Low Byte)
    outb(0x1F2, 0x00); // البايت العالي لعدد القطاعات (0 قطاع إضافي)
    outb(0x1F2, 0x01); // البايت المنخفض لعدد القطاعات (قطاع واحد)

    // 4. إرسال عنوان القطاع (LBA) المكون من 48 بت على 3 منافذ (كل منفذ يستقبل بايتين)
    
    // منفذ LBA Low (0x1F3)
    outb(0x1F3, (unsigned char)((sector_num >> 24) & 0xFF)); // البايت الرابع (Bits 24-31)
    outb(0x1F3, (unsigned char)(sector_num & 0xFF));         // البايت الأول  (Bits 0-7)

    // منفذ LBA Mid (0x1F4)
    outb(0x1F4, (unsigned char)((sector_num >> 32) & 0xFF)); // البايت الخامس (Bits 32-39)
    outb(0x1F4, (unsigned char)((sector_num >> 8) & 0xFF));  // البايت الثاني  (Bits 8-15)

    // منفذ LBA High (0x1F5)
    outb(0x1F5, (unsigned char)((sector_num >> 40) & 0xFF)); // البايت السادس (Bits 40-47)
    outb(0x1F5, (unsigned char)((sector_num >> 16) & 0xFF)); // البايت الثالث  (Bits 16-23)

    // 5. إرسال أمر القراءة الخاص بـ LBA 48 EXTENDED
    outb(0x1F7, 0x24); // الأمر 0x24 هو (READ SECTORS EXT)

    // 6. الانتظار (Polling) حتى تجهز البيانات
    while ((inb(0x1F7) & 0x80) != 0); // انتظر اختفاء Busy
    while ((inb(0x1F7) & 0x08) == 0); // انتظر ظهور DRQ (طلب البيانات)

    // 7. قراءة الـ 512 بايت (256 كلمة) من منفذ البيانات
    for (int i = 0; i < 256; i++) {
        sector[i] = inw(0x1F0);
    }

    return sector;
}

void ata_write_sector(unsigned long long sector_num, unsigned short *sector_contect) {
    // 1. الانتظار حتى يصبح القرص غير مشغول (BSY == 0)
    while ((inb(0x1F7) & 0x80) != 0);
    ata_init();

    // 2. إرسال سجل المحرك (Drive Select)
    // في نمط LBA 48، نرسل القيمة 0x40 فقط في البت السادس لتفعيل LBA والـ Master
    // (لا ندمج أي بتات من عنوان القطاع في هذا المنفذ كما كنا نفعل في LBA 28)
    outb(0x1F6, 0x40); 

    // 3. إرسال عدد القطاعات (Sector Count) - وهو 16 بت في LBA 48
    // نرسل البايت العالي (High Byte) ثم البايت المنخفض (Low Byte)
    outb(0x1F2, 0x00); // البايت العالي لعدد القطاعات (0 قطاع إضافي)
    outb(0x1F2, 0x01); // البايت المنخفض لعدد القطاعات (قطاع واحد)

    // 4. إرسال عنوان القطاع (LBA) المكون من 48 بت على 3 منافذ (كل منفذ يستقبل بايتين)
    
    // منفذ LBA Low (0x1F3)
    outb(0x1F3, (unsigned char)((sector_num >> 24) & 0xFF)); // البايت الرابع (Bits 24-31)
    outb(0x1F3, (unsigned char)(sector_num & 0xFF));         // البايت الأول  (Bits 0-7)

    // منفذ LBA Mid (0x1F4)
    outb(0x1F4, (unsigned char)((sector_num >> 32) & 0xFF)); // البايت الخامس (Bits 32-39)
    outb(0x1F4, (unsigned char)((sector_num >> 8) & 0xFF));  // البايت الثاني  (Bits 8-15)

    // منفذ LBA High (0x1F5)
    outb(0x1F5, (unsigned char)((sector_num >> 40) & 0xFF)); // البايت السادس (Bits 40-47)
    outb(0x1F5, (unsigned char)((sector_num >> 16) & 0xFF)); // البايت الثالث  (Bits 16-23)

    // 5. إرسال أمر القراءة الخاص بـ LBA 48 EXTENDED
    outb(0x1F7, 0x34); // الأمر 0x24 هو (READ SECTORS EXT)

    // 6. الانتظار (Polling) حتى تجهز البيانات
    while ((inb(0x1F7) & 0x80) != 0); // انتظر اختفاء Busy
    //while ((inb(0x1F7) & 0x08) == 0); // انتظر ظهور DRQ (طلب البيانات)

    // 7. قراءة الـ 512 بايت (256 كلمة) من منفذ البيانات
    for (int i = 0; i < 256; i++) {
        outw(ATA_PIO_DATA_PORT, sector_contect[i]);
    }

    outb(ATA_PIO_COMMAND_PORT, 0xE7);
    while ((inb(0x1F7) & 0x80) != 0); // انتظر اختفاء Busy
}