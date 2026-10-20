#include <vars.h>
#include <types.h>
#include <fb.h>
#include <alifs.h>

static alifs_root_sector_header_t alifs_root_header = {0};

static char *alifs_disk_start = NULL;

// دالة مساعدة لتحويل الرقم العشري إلى نص ثماني متوافق مع TAR
void int_to_octal(unsigned int num, char *out_str, int size) {
    // ملء المصفوفة بأكملها بـ '0' أولاً
    for (int i = 0; i < size - 1; i++) {
        out_str[i] = '0';
    }
    out_str[size - 1] = '\0'; // المحرف الصِفري القياسي في النهاية

    int idx = size - 2; // نبدأ الكتابة من اليمين إلى اليسار (قبل الـ Null)
    if (num == 0) return; // إذا كان صفرًا يظل كله أصفارًا

    while (num > 0 && idx >= 0) {
        out_str[idx--] = (num % 8) + '0';
        num /= 8;
    }
}

// دالة مساعدة موحدة لقراءة الحجم من الهيدر (لتجنب تكرار الكود)
unsigned int get_tar_sum_of_size() {
    char *alifs_file = alifs_disk_start + 512;
    unsigned int size = 0;
    unsigned int sum = 0;
    while (alifs_file[0] != '\0') {
        for (int i = 0; i < 11; i++) {
            if (alifs_file[i + 124] >= '0' && alifs_file[i + 124] <= '7') {
                size = (size << 3) + (alifs_file[i + 124] - '0');
            }
        }
        sum += size;
        alifs_file += 512 + ((size + 511) & ~511);
    }
    return sum;
}

void alifs_init(char *alifs_start) {
    alifs_disk_start = alifs_start - 512;
    if (memcmp(alifs_disk_start, "/\0\x02", 3) != 0) {
        alifs_disk_start += 512;
        if (memcmp(alifs_disk_start, "/\0\x02", 3) != 0) {
            alifs_disk_start -= 512;
            alifs_disk_start[0] = '/';
            alifs_disk_start[1] = '\0';
            alifs_disk_start[2] = 2; // typeflag
            //Actual_part_size
            alifs_disk_start[3] = 8;
            alifs_disk_start[4] = 0;
            alifs_disk_start[5] = 0;
            alifs_disk_start[6] = 0;
            //Used_size
            alifs_disk_start[7] = get_tar_sum_of_size() & 0xFF;
            alifs_disk_start[8] = (get_tar_sum_of_size() >> 8) & 0xFF;
            alifs_disk_start[9] = (get_tar_sum_of_size() >> 16) & 0xFF;
            alifs_disk_start[10] = (get_tar_sum_of_size() >> 24) & 0xFF;
            //Linkname
            alifs_disk_start[11] = 0;
            alifs_disk_start[12] = 0;
            alifs_disk_start[13] = 0;
        } else {
            return;
        }
    } else {
        return;
    }
}

void alifs_dir()
{
    unsigned short i = 0;
    alifs_root_header = *(alifs_root_sector_header_t *)alifs_disk_start;
    while (alifs_root_header.files_and_dirs[16 * i] != '\0')
    {
    alifs_header_t alifs_header = *(alifs_header_t *)alifs_root_header.files_and_dirs;
    fb_puts(alifs_header.name);
    for (int i = 0; i < (32 - strlen(alifs_header.name)); i++)
    {
        fb_putc(' ');
    }
    
    if ((alifs_header.typeflag - '0') == 0) {
        fb_puts(int_to_str((int)alifs_header.sum_of_size));
        fb_putc('\t');
    } else if ((alifs_header.typeflag - '0') == 5) {
        fb_puts("<DIR>\t");
    }
    fb_putc('\n');
    i++; // الانتقال إلى رأس التار التالي
    }
}
/*
void tar_ls()
{
    unsigned short i = 0;
    char *tar_file = tar_disk_start;
    while (tar_file[0] != '\0')
    {
    tar_header = *(tar_header_t *)tar_file;
    unsigned int size = 0;

    for (unsigned char i = 0; i < 11; i++)
    {
        // 1. تخطي المسافات أو الأصفار الممتلئة في البداية والنهاية
    if (tar_header.size_raw[i] == ' ' || tar_header.size_raw[i] == '\0') {
        continue; 
    }
    
    // 2. التحقق من أن الحرف هو رقم ثماني صالح (بين '0' و '7')
    if (tar_header.size_raw[i] >= '0' && tar_header.size_raw[i] <= '7') {
            // إزاحة المجموع الحالي (الضرب في 8) ثم إضافة القيمة الرقمية الجديدة
            size = (size << 3) + (tar_header.size_raw[i] - '0');
        }
    }
    if ((tar_header.type - '0') == 0) {
        fb_puts(tar_header.name);
        fb_putc(' ');
    } else if ((tar_header.type - '0') == 5) {
        fb_putc('[');
        fb_puts(tar_header.name);
        fb_puts("] ");
    }
    if (((i + 1) % 5) == 0) fb_putc('\n');
    i++;
    tar_file += (512 + size + 511) & ~511; // الانتقال إلى رأس التار التالي
    }
    fb_putc('\n');
}

char *tar_read(char *file_name) {
    char *tar_file = tar_disk_start;
    tar_header_t *header = (tar_header_t *)tar_file;
    
    while (tar_file[0] != '\0') {
        if (strcmp(header->name, file_name) == 0) {
            // وجدنا الملف
            unsigned int size = 0;
            for (int i = 0; i < 11; i++) {
                if (header->size_raw[i] >= '0' && header->size_raw[i] <= '7') {
                    size = (size << 3) + (header->size_raw[i] - '0');
                }
            }
            if (header->type == '5') {  // دليل
                return NULL;
            }
            return tar_file + 512;  // بيانات الملف
        }
        // لم نجده، اقفز للملف التالي
        unsigned int size = 0;
        for (int i = 0; i < 11; i++) {
            if (header->size_raw[i] >= '0' && header->size_raw[i] <= '7') {
                size = (size << 3) + (header->size_raw[i] - '0');
            }
        }
        tar_file += 512 + ((size + 511) & ~511);
        header = (tar_header_t *)tar_file;
    }
    return NULL;
}

int tar_create(char *file_name, unsigned int alloc_size)
{
    char *tar_file = tar_disk_start;
    // 1. البحث عن نهاية الملفات الحالية والوصول لكتلة الأصفار
    while (tar_file[0] != '\0')
    {
        tar_header_t *tar_header = (tar_header_t *)tar_file;
        if (strcmp(tar_header->name, file_name) == 0)
        {
            fb_puts("Error: ");
            fb_puts(file_name);
            fb_puts(" already exists in the tar file.\n");
            return -1;
        }
        unsigned int size = get_tar_size(file_name); // الدالة المساعدة لحساب الحجم
        tar_file += 512 + ((size + 511) & ~511);
    }

    // 2. تصفير الـ 512 بايت الخاصة بالهيدر الجديد تماماً للتخلص من أي قيم قديمة في الذاكرة
    for (int i = 0; i < 512; i++) {
        tar_file[i] = 0;
    }

    // 3. الكتابة داخل المؤشر مباشرة لتجنب مشاكل النسخ بالـ قيمة
    tar_header_t *new_header = (tar_header_t *)tar_file;
    
    // نسخ الاسم يدوياً أو عبر strcopy
    int name_len = 0;
    while (file_name[name_len] != '\0' && name_len < 99) {
        new_header->name[name_len] = file_name[name_len];
        name_len++;
    }
    new_header->name[name_len] = '\0';

    // تعبئة الحقول الأساسية بنصوص ثمانية نظيفة
    int_to_octal(alloc_size, new_header->size_raw, 12);
    int_to_octal(0, new_header->mode, 8);
    int_to_octal(0, new_header->uid, 8);
    int_to_octal(0, new_header->gid, 8);
    int_to_octal(0, new_header->mtime, 12);
    
    new_header->type = '0'; // ملف عادي

    // حساب الـ Checksum (ضروري جداً إذا كنت ستقرأ الأرشيف خارج الـ Kernel لاحقاً)
    unsigned int chksum = 0;
    char *header_bytes = (char *)tar_file;
    for (int i = 0; i < 512; i++) {
        if (i >= 148 && i < 156) chksum += ' '; // حقل chksum يحسب كمسافات
        else chksum += (unsigned char)header_bytes[i];
    }
    int_to_octal(chksum, new_header->chksum, 8);

    // 4. الانتقال لمساحة البيانات وتصفيرها بالكامل لحجز المساحة
    char *data_area = tar_file + 512;
    unsigned int aligned_data_size = (alloc_size + 511) & ~511;
    for (unsigned int i = 0; i < aligned_data_size; i++) {
        data_area[i] = 0;
    }

    // 5. وضع علامة نهاية الأرشيف الجديدة (1024 بايت من الأصفار) بعد الملف مباشرة
    char *end_of_archive = data_area + aligned_data_size;
    for (int i = 0; i < 1024; i++) {
        end_of_archive[i] = 0;
    }

    return 0;
}

int tar_write(char *file_name, char *data, unsigned int data_size)
{
    char *tar_file = tar_disk_start;
    // نضمن أن المؤشر يبدأ من أول الأرشيف ويتحرك بايت ببايت
    while (tar_file[0] != '\0')
    {
        tar_header_t *current_header = (tar_header_t *)tar_file;
        
        // هل هذا هو الملف المطلوب؟
        if (strcmp(current_header->name, file_name) == 0)
        {
            unsigned int allocated_size = get_tar_size(file_name);
            
            // التحقق من أن حجم البيانات المكتوبة لا يتخطى المساحة المحجوزة في الـ create
            /*
            if (data_size > allocated_size)
            {
                fb_puts("Error: Data size exceeds the allocated size for ");
                fb_puts(file_name);
                fb_puts(".\n");
                return -1;
            }
            

            // مؤشر بداية منطقة البيانات (بعد الـ Header بـ 512 بايت)
            char *file_data_start = tar_file + 512;
            
            // 1. كتابة البيانات الجديدة
            for (unsigned int i = 0; i < data_size; i++)
            {
                file_data_start[i] = data[i];
            }
            
            // 2. تصفير المساحة المتبقية داخل البلوك المحجوز (مهم جداً لسلامة الـ TAR)
            unsigned int total_aligned_size = (allocated_size + 511) & ~511;
            for (unsigned int i = data_size; i < total_aligned_size; i++)
            {
                file_data_start[i] = 0;
            }

            // نجحت الكتابة واستقر الهيكل
            return 0;
        }

        // إذا لم يكن الملف المطلوب، نقفز للملف التالي بناءً على حجمه
        unsigned int size = get_tar_size(file_name);
        tar_file += 512 + ((size + 511) & ~511);
    }
    
    fb_puts("Error: ");
    fb_puts(file_name);
    fb_puts(" not found in the tar file.\n");
    return -1;
}

int exec(char *file_name)
{
    // 1. قراءة الملف من tar
    char *file_data = tar_read(file_name);

    unsigned int size = get_tar_size(file_name);
    

    // 2. العنوان المستهدف في Real Mode (0x0600 منطقة آمنة)
    unsigned int address = 0x0600;
    unsigned char *dest = (unsigned char *)address;

    // 3. نسخ البيانات
    for (unsigned int i = 0; i < size; i++) {
        dest[i] = file_data[i];
    }

    // 4. تنفيذ البرنامج
    // ملاحظة: لا نغيّر segments، البرنامج سيرث CS/DS/SS الحالية
    typedef void (*call_t)();
    call_t prog = (call_t) address;
    fb_puts("Executing ...\n");
    prog();
    fb_puts("Program returned to kernel.\n");

    return 0;
}
*/