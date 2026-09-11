#include <io.h>
#include <types.h>
// دالة لتغيير الخط
void vga_load_font(uint8_t *font_matrix)
{
    // 1. فتح التحكم في سجلات السلسلة (Sequencer) وسجلات الجرافيكس
    outb(0x3C4, 0x00); outb(0x3C5, 0x01); // إيقاف متزامن لإعادة البرمجة
    outb(0x3C4, 0x02); outb(0x3C5, 0x04); // الكتابة فقط على Plane 2
    outb(0x3C4, 0x04); outb(0x3C5, 0x07); // تفعيل وضع الوصول المتسلسل
    outb(0x3C4, 0x00); outb(0x3C5, 0x03); // إعادة التشغيل المتزامن
    
    outb(0x3CE, 0x04); outb(0x3CF, 0x02); // قراءة من Plane 2
    outb(0x3CE, 0x05); outb(0x3CF, 0x00); // تفعيل الوضع العادي للقرائة والكتابة
    outb(0x3CE, 0x06); outb(0x3CF, 0x00); // تحديد بداية ذاكرة الجرافيكس عند 0xA0000

    // 2. نسخ بيانات الخط إلى ذاكرة الفيديو (Plane 2 تبدأ من 0xA0000 في هذا الوضع)
    uint8_t *vga_mem = (uint8_t *)0xA0000;
    
    // مصفوفة الخطوط تحتوي عادة على 256 حرفًا، وكل حرف يأخذ 32 بايت في الذاكرة (نستخدم أول 16 منها)
    for (int count = 0; count < 256; count++) {
        for (int i = 0; i < 16; i++) {
            vga_mem[(count * 32) + i] = font_matrix[(count * 16) + i];
        }
    }

    // 3. إعادة إعدادات VGA لوضع النصوص الطبيعي (Text Mode) لتظهر الأحرف مجدداً
    outb(0x3C4, 0x00); outb(0x3C5, 0x01); // إيقاف متزامن
    outb(0x3C4, 0x02); outb(0x3C5, 0x03); // الكتابة على Plane 0 و 1 (الأحرف والألوان)
    outb(0x3C4, 0x04); outb(0x3C5, 0x03); // إعادة تفعيل البنية الهيكلية لوضع النصوص
    outb(0x3C4, 0x00); outb(0x3C5, 0x03); // إعادة التشغيل المتزامن
    
    outb(0x3CE, 0x04); outb(0x3CF, 0x00); // العودة للقراءة الطبيعية
    outb(0x3CE, 0x05); outb(0x3CF, 0x10); // العودة لوضع النصوص
    outb(0x3CE, 0x06); outb(0x3CF, 0x0E); // إعادة توجيه الذاكرة النصية إلى 0xB8000
    /*
    // إخبار الـ VGA باستدعاء الـ 512 حرفاً (تقسيم الذاكرة عتادياً بين Font A و Font B)
    outb(0x3C4, 0x03); 
    outb(0x3C5, 0x04); // القيمة 0x04 تقوم بتفعيل كلا الخطين معاً عتادياً

    inb(0x3DA);              // قراءة السجل لإعادة تصفير مؤشر الـ Attribute Controller
    outb(0x3C0, 0x10 | 0x20); // اختيار سجل التحكم بالوضع (Mode Control)
    uint8_t mode = inb(0x3C1);
    mode |= 0x01;            // تفعيل البت المسؤول عن ميزة الـ 512 حرف (Line Graphics Mode)
    outb(0x3C0, mode);       // حفظ القيمة الجديدة
    */
}

void test_vga_font(void) {
    // مصفوفة ثابتة لحرف مخصص (شكل مربع خفيف) لنعتمد عليها بدلاً من ملف الـ Dump
    uint8_t custom_box[16] = {
        0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
        0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF
    };

    // 1. فتح الـ VGA للخطوط
    outb(0x3C4, 0x00); outb(0x3C5, 0x01); 
    outb(0x3C4, 0x02); outb(0x3C5, 0x04); 
    outb(0x3C4, 0x04); outb(0x3C5, 0x07); 
    outb(0x3C4, 0x00); outb(0x3C5, 0x03); 
    
    outb(0x3CE, 0x04); outb(0x3CF, 0x02); 
    outb(0x3CE, 0x05); outb(0x3CF, 0x00); 
    outb(0x3CE, 0x06); outb(0x3CF, 0x00); 

    // 2. ملء ذاكرة الـ VGA بالكامل (256 حرف) بهذا الشكل الثابت
    uint8_t *vga_mem = (uint8_t *)0xA0000;
    for (int count = 0; count < 256; count++) {
        for (int i = 0; i < 16; i++) {
            vga_mem[(count * 32) + i] = custom_box[i];
        }
    }

    // 3. إغلاق الـ VGA والعودة لوضع النصوص
    outb(0x3C4, 0x00); outb(0x3C5, 0x01); 
    outb(0x3C4, 0x02); outb(0x3C5, 0x03); 
    outb(0x3C4, 0x04); outb(0x3C5, 0x03); 
    outb(0x3C4, 0x00); outb(0x3C5, 0x03); 
    
    outb(0x3CE, 0x04); outb(0x3CF, 0x00); 
    outb(0x3CE, 0x05); outb(0x3CF, 0x10); 
    outb(0x3CE, 0x06); outb(0x3CF, 0x0E); 
}