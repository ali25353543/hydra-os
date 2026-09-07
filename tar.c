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
    if ((tar_header.type[0] - '0') == 0) {
        fb_puts(int_to_str((int)size));
        fb_putc('\t');
    } else if ((tar_header.type[0] - '0') == 5) {
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
    return tar_file + 512;
}