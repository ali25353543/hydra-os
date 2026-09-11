#include <types.h>
#include <fb.h>
#include <tar.h>

static tar_header_t tar_header = {0};

void tar_info(char *tar_file)
{
    while (tar_file[0] != '\0')
    {
    tar_header = *(tar_header_t *)tar_file;
    fb_puts(tar_header.name);
    for (int i = 0; i < (32 - strlen(tar_header.name)); i++)
    {
        fb_putc(' ');
    }
    
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
        fb_puts(int_to_str((int)size));
        fb_putc('\t');
    } else if ((tar_header.type - '0') == 5) {
        fb_puts("<DIR>\t");
    }
    
    fb_putc('\n');
    tar_file += (512 + size + 511) & ~511; // الانتقال إلى رأس التار التالي
    }
}

char *tar_read(char *tar_file, char *file_name)
{
    tar_header_t tar_header = *(tar_header_t *)tar_file;
    unsigned int size = 0;
    while (tar_file[0] != '\0' && strcmp(tar_header.name, file_name) != 0){
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
        tar_file += (512 + size + 511) & ~511;
        tar_header = *(tar_header_t *)tar_file;
    }
    size = 0;
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
    if (tar_header.type - '0' == 5) {
        fb_puts("Error: ");
        fb_puts(file_name);
        fb_puts(" is a directory.\n");
        return (char *)NULL;
    }
    return tar_file + 512;
}

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
unsigned int get_tar_size(char *tar_file, char *file_name) {
    unsigned int size = 0;
    while (tar_file[0] != 0 && strcmp(tar_file, file_name) != 0)
    {
        tar_header = *(tar_header_t *)tar_file;
    for (unsigned char i = 0; i < 11; i++) {
        if (header->size_raw[i] == ' ' || header->size_raw[i] == '\0') {
            continue; 
        }
        if (header->size_raw[i] >= '0' && header->size_raw[i] <= '7') {
            size = (size << 3) + (header->size_raw[i] - '0');
        }
    }
    }
    return size;
}

int tar_create(char *tar_file, char *file_name, unsigned int alloc_size)
{
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
        unsigned int size = get_tar_size(tar_header); // الدالة المساعدة لحساب الحجم
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

int tar_write(char *tar_file, char *file_name, char *data, unsigned int data_size)
{
    // نضمن أن المؤشر يبدأ من أول الأرشيف ويتحرك بايت ببايت
    while (tar_file[0] != '\0')
    {
        tar_header_t *current_header = (tar_header_t *)tar_file;
        
        // هل هذا هو الملف المطلوب؟
        if (strcmp(current_header->name, file_name) == 0)
        {
            unsigned int allocated_size = get_tar_size(current_header);
            
            // التحقق من أن حجم البيانات المكتوبة لا يتخطى المساحة المحجوزة في الـ create
            /*
            if (data_size > allocated_size)
            {
                fb_puts("Error: Data size exceeds the allocated size for ");
                fb_puts(file_name);
                fb_puts(".\n");
                return -1;
            }
            */

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
        unsigned int size = get_tar_size(current_header);
        tar_file += 512 + ((size + 511) & ~511);
    }
    
    fb_puts("Error: ");
    fb_puts(file_name);
    fb_puts(" not found in the tar file.\n");
    return -1;
}

int exec(char *tar_file, char *file_name, int *args)
{
    char *file_data = tar_read(tar_file, file_name);
    if (file_data == NULL) {
        fb_puts("Error: Unable to read the file for execution.\n");
        return -1;
    }
    char *data = file_data;
    // Assuming the file is a binary executable, we would typically jump to its entry point.
    // However, in this context, we will just print a message indicating execution.
    fb_puts("Executing ");
    fb_puts(file_name);
    fb_puts("...\n");
    if (strncmp(file_data, "Hydra OS", 8) != 0) {
        fb_puts("Invalid Hydra OS program.\n");
        return -1;
    }
    file_data += 9; // Skip the "Hydra OS" signature
    unsigned int size = get_tar_size((tar_header_t *)(tar_file));
    unsigned int address = (unsigned int)file_data;
    file_data += 4; // Skip the size field
    if (address < 0x40000000) {
        fb_puts("Error: Invalid execution address.\n");
        return -1;
    }
    for (int i = 0; i < (int)size; i++)
    {
        ((char *)address)[i] = data[i];
    }
    ((void (*)(int *))address)(args); // Call the function pointer to execute the code
    return 0; // Indicate success
}