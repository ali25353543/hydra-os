#include "ata.h"
#include "fat32.h"

/*                       المتغيرات العامة الثابتة                             */
/* ========================================================================== */

unsigned int partition_start_sector = 0;
unsigned int first_data_sector = 0;

unsigned short bytes_per_sector = 512;
unsigned char  sectors_per_cluster = 0;
unsigned short reserved_sector_count = 0;
unsigned char  table_count = 0;
unsigned int   table_size_32 = 0;
unsigned int   root_cluster = 0;

// دالة لتهيئة نظام الملفات واستخراج القيم بالبايت المفرد (حل قاطع لتشوه الأرقام)
void init_fat32_file_system(unsigned int partition_lba_start) {
    partition_start_sector = partition_lba_start;
    
    unsigned char *boot_buf = (unsigned char *)ata_read_sector(partition_start_sector);

    // استخراج القيم بايت تلو بايت يدوياً (Bit Shifting) لمنع مشاكل الـ Alignment تماماً
    bytes_per_sector      = (unsigned short)boot_buf[11] | ((unsigned short)boot_buf[12] << 8);
    sectors_per_cluster   = boot_buf[13];
    reserved_sector_count = (unsigned short)boot_buf[14] | ((unsigned short)boot_buf[15] << 8);
    table_count           = boot_buf[16];
    
    // استخراج حجم الـ FAT المكون من 4 بايت بدقة شديدة
    table_size_32         = (unsigned int)boot_buf[36] | 
                            ((unsigned int)boot_buf[37] << 8) | 
                            ((unsigned int)boot_buf[38] << 16) | 
                            ((unsigned int)boot_buf[39] << 24);
                            
    root_cluster          = (unsigned int)boot_buf[44] | 
                            ((unsigned int)boot_buf[45] << 8) | 
                            ((unsigned int)boot_buf[46] << 16) | 
                            ((unsigned int)boot_buf[47] << 24);

    // جدار حماية حرج لحماية الـ MBR في حال فشل القراءة
    if (bytes_per_sector != 512 || table_count > 2 || table_count == 0 || table_size_32 == 0) {
        bytes_per_sector = 512;
        sectors_per_cluster = 8;
        reserved_sector_count = 32;
        table_count = 2;
        table_size_32 = 1024; // قيمة افتراضية آمنة تضمن تشغيل حلقة البحث
        root_cluster = 2;
    }

    first_data_sector = partition_start_sector + reserved_sector_count + (table_count * table_size_32);
}

static unsigned int cluster_to_sector(unsigned int cluster) {
    if (cluster < 2) return 0;
    return first_data_sector + ((cluster - 2) * sectors_per_cluster);
}

static unsigned int get_next_cluster(unsigned int current_cluster) {
    unsigned int fat_start_sector = partition_start_sector + reserved_sector_count;
    unsigned int fat_offset = current_cluster * 4;
    unsigned int sector = fat_start_sector + (fat_offset / 512);
    unsigned int index = (fat_offset % 512) / 4;

    unsigned char sector_buffer[512];
    unsigned char *p = (unsigned char *)ata_read_sector(sector);
    for(int b = 0; b < 512; b++) sector_buffer[b] = p[b];

    // استخراج آمن للقيمة لتفادي الـ Alignment Issue
    unsigned int entry_val = (unsigned int)sector_buffer[index * 4] |
                             ((unsigned int)sector_buffer[index * 4 + 1] << 8) |
                             ((unsigned int)sector_buffer[index * 4 + 2] << 16) |
                             ((unsigned int)sector_buffer[index * 4 + 3] << 24);

    return entry_val & 0x0FFFFFFF;
}

static int parse_linux_path(const char *path, char *fat_name_out) {
    if (path[0] != '/') return -1; 
    const char *filename = &path[1]; 
    
    for (int i = 0; i < 11; i++) fat_name_out[i] = ' ';

    int i = 0;
    while (filename[i] != '\0' && filename[i] != '.' && i < 8) {
        char c = filename[i];
        if (c >= 'a' && c <= 'z') c -= 32; 
        fat_name_out[i] = c;
        i++;
    }

    int dot_index = -1;
    for (int k = 0; filename[k] != '\0'; k++) {
        if (filename[k] == '.') { dot_index = k + 1; break; }
    }
    
    if (dot_index != -1) {
        int ext_idx = 8;
        int j = dot_index;
        while (filename[j] != '\0' && ext_idx < 11) {
            char c = filename[j];
            if (c >= 'a' && c <= 'z') c -= 32;
            fat_name_out[ext_idx] = c;
            j++; ext_idx++;
        }
    }
    return 0; 
}

static unsigned int find_free_cluster(void) {
    unsigned int fat_start_sector = partition_start_sector + reserved_sector_count;
    unsigned char sector_buffer[512];

    for (unsigned int s = 0; s < table_size_32; s++) {
        unsigned char *p = (unsigned char *)ata_read_sector(fat_start_sector + s);
        for(int b = 0; b < 512; b++) sector_buffer[b] = p[b];

        for (int i = 0; i < 128; i++) {
            int idx = i * 4;
            // قراءة البايتات بشكل آمن تماماً
            unsigned int cluster_val = (unsigned int)sector_buffer[idx] |
                                       ((unsigned int)sector_buffer[idx + 1] << 8) |
                                       ((unsigned int)sector_buffer[idx + 2] << 16) |
                                       ((unsigned int)sector_buffer[idx + 3] << 24);
                                       
            cluster_val &= 0x0FFFFFFF;
            unsigned int current_cluster = (s * 128) + i;
            
            if (cluster_val == 0 && current_cluster >= 2) {
                return current_cluster;
            }
        }
    }
    return 0;
}

static void write_fat_entry(unsigned int cluster, unsigned int value) {
    unsigned int fat_start_sector = partition_start_sector + reserved_sector_count;
    unsigned int fat_offset = cluster * 4;
    unsigned int sector = fat_start_sector + (fat_offset / 512);
    unsigned int index = (fat_offset % 512) / 4;
    int idx = index * 4;

    unsigned char sector_buffer[512];
    unsigned char *p = (unsigned char *)ata_read_sector(sector);
    for(int b = 0; b < 512; b++) sector_buffer[b] = p[b];

    // الاحتفاظ بأعلى 4 بتات وحقن الـ 28 بت القادمة بشكل آمن بالبايت
    unsigned int current_val = (unsigned int)sector_buffer[idx] |
                               ((unsigned int)sector_buffer[idx + 1] << 8) |
                               ((unsigned int)sector_buffer[idx + 2] << 16) |
                               ((unsigned int)sector_buffer[idx + 3] << 24);
                               
    unsigned int new_val = (current_val & 0xF0000000) | (value & 0x0FFFFFFF);

    // توزيع بايتات القيمة الجديدة داخل الـ buffer
    sector_buffer[idx]     = (unsigned char)(new_val & 0xFF);
    sector_buffer[idx + 1] = (unsigned char)((new_val >> 8) & 0xFF);
    sector_buffer[idx + 2] = (unsigned char)((new_val >> 16) & 0xFF);
    sector_buffer[idx + 3] = (unsigned char)((new_val >> 24) & 0xFF);

    ata_write_sector(sector, (unsigned short *)sector_buffer);
}

static unsigned int allocate_cluster_chain(unsigned int num_clusters_needed) {
    unsigned int first_allocated_cluster = 0;
    unsigned int last_allocated_cluster = 0;

    for (unsigned int i = 0; i < num_clusters_needed; i++) {
        unsigned int free_cluster = find_free_cluster();
        if (free_cluster == 0) return 0; 

        write_fat_entry(free_cluster, 0x0FFFFFFF);

        if (first_allocated_cluster == 0) {
            first_allocated_cluster = free_cluster;
        } else {
            write_fat_entry(last_allocated_cluster, free_cluster);
        }
        last_allocated_cluster = free_cluster;
    }
    return first_allocated_cluster;
}

int sys_create_in_root(const char *linux_path, const unsigned char *data, unsigned int size) {
    if (first_data_sector == 0 || sectors_per_cluster == 0) return -5;

    char fat_name[11];
    if (parse_linux_path(linux_path, fat_name) != 0) return -1;

    unsigned int free_cluster = find_free_cluster();
    if (free_cluster == 0) return -2; 

    unsigned int file_sector = cluster_to_sector(free_cluster);
    if (file_sector == 0 || file_sector < first_data_sector) return -3;

    write_fat_entry(free_cluster, 0x0FFFFFFF); 

    unsigned char file_buffer[512] = {0};
    unsigned int bytes_to_write = (size > 512) ? 512 : size;
    for (unsigned int i = 0; i < bytes_to_write; i++) file_buffer[i] = data[i];
    ata_write_sector(file_sector, (unsigned short *)file_buffer);

    unsigned int dir_sector = cluster_to_sector(root_cluster);
    if (dir_sector == 0 || dir_sector < first_data_sector) return -4;

    unsigned char dir_buffer[512];
    unsigned char *p = (unsigned char *)ata_read_sector(dir_sector);
    for(int b = 0; b < 512; b++) dir_buffer[b] = p[b];

    for (int i = 0; i < 16; i++) {
        int entry_offset = i * 32;
        if (dir_buffer[entry_offset] == 0x00 || dir_buffer[entry_offset] == 0xE5) {
            for (int n = 0; n < 11; n++) dir_buffer[entry_offset + n] = fat_name[n];
            dir_buffer[entry_offset + 11] = 0x20; 
            for (int r = 12; r < 26; r++) dir_buffer[entry_offset + r] = 0;

            unsigned short hi_clus = (unsigned short)((free_cluster >> 16) & 0xFFFF);
            unsigned short lo_clus = (unsigned short)(free_cluster & 0xFFFF);
            
            dir_buffer[entry_offset + 26] = (unsigned char)(hi_clus & 0xFF);
            dir_buffer[entry_offset + 27] = (unsigned char)((hi_clus >> 8) & 0xFF);
            dir_buffer[entry_offset + 28] = (unsigned char)(lo_clus & 0xFF);
            dir_buffer[entry_offset + 29] = (unsigned char)((lo_clus >> 8) & 0xFF);

            dir_buffer[entry_offset + 30] = (unsigned char)(bytes_to_write & 0xFF);
            dir_buffer[entry_offset + 31] = (unsigned char)((bytes_to_write >> 8) & 0xFF);
            dir_buffer[entry_offset + 32] = (unsigned char)((bytes_to_write >> 16) & 0xFF);
            dir_buffer[entry_offset + 33] = (unsigned char)((bytes_to_write >> 24) & 0xFF);

            ata_write_sector(dir_sector, (unsigned short *)dir_buffer);
            return 0;
        }
    }
    return -6; 
}

int sys_create_in_root_large(const char *linux_path, const unsigned char *data, unsigned int size) {
    if (first_data_sector == 0 || sectors_per_cluster == 0) return -5;
    char fat_name[11];
    if (parse_linux_path(linux_path, fat_name) != 0) return -1;
    unsigned int bytes_per_cluster = sectors_per_cluster * 512;
    unsigned int num_clusters_needed = (size + bytes_per_cluster - 1) / bytes_per_cluster;
    unsigned int first_cluster = allocate_cluster_chain(num_clusters_needed);
    if (first_cluster == 0) return -2;
    unsigned int current_cluster = first_cluster;
    unsigned int bytes_remaining = size;
    unsigned int data_offset = 0;
    while (bytes_remaining > 0 && current_cluster < 0x0FFFFFF8) {
    unsigned int start_sector = cluster_to_sector(current_cluster);
    if (start_sector == 0 || start_sector < first_data_sector) return -3;
    for (int s = 0; s < sectors_per_cluster; s++) {
    if (bytes_remaining == 0) break;
    unsigned char file_buffer[512] = {0};
    unsigned int chunk = (bytes_remaining > 512) ? 512 : bytes_remaining;
    for (unsigned int i = 0; i < chunk; i++) {
    file_buffer[i] = data[data_offset + i];
    }
    ata_write_sector(start_sector + s, (unsigned short *)file_buffer);
    bytes_remaining -= chunk;
    data_offset += chunk;
    }
    current_cluster = get_next_cluster(current_cluster);
    }
    unsigned int dir_sector = cluster_to_sector(root_cluster);
    if (dir_sector == 0 || dir_sector < first_data_sector) return -4;
    unsigned char dir_buffer[512];
    unsigned char *p = (unsigned char *)ata_read_sector(dir_sector);
    for(int b = 0; b < 512; b++) dir_buffer[b] = p[b];
    for (int i = 0; i < 16; i++) {
    int entry_offset = i * 32;
    if (dir_buffer[entry_offset] == 0x00 || dir_buffer[entry_offset] == 0xE5) {
    for (int n = 0; n < 11; n++) dir_buffer[entry_offset + n] = fat_name[n];
    dir_buffer[entry_offset + 11] = 0x20;
    for (int r = 12; r < 26; r++) dir_buffer[entry_offset + r] = 0;
    unsigned short hi_clus = (unsigned short)((first_cluster >> 16) & 0xFFFF);
    unsigned short lo_clus = (unsigned short)(first_cluster & 0xFFFF);
    dir_buffer[entry_offset + 26] = (unsigned char)(hi_clus & 0xFF);
    dir_buffer[entry_offset + 27] = (unsigned char)((hi_clus >> 8) & 0xFF);
    dir_buffer[entry_offset + 28] = (unsigned char)(lo_clus & 0xFF);
    dir_buffer[entry_offset + 29] = (unsigned char)((lo_clus >> 8) & 0xFF);
    dir_buffer[entry_offset + 30] = (unsigned char)(size & 0xFF);
    dir_buffer[entry_offset + 31] = (unsigned char)((size >> 8) & 0xFF);
    dir_buffer[entry_offset + 32] = (unsigned char)((size >> 16) & 0xFF);
    dir_buffer[entry_offset + 33] = (unsigned char)((size >> 24) & 0xFF);
    ata_write_sector(dir_sector, (unsigned short *)dir_buffer);
    return 0;
    }
    }
    return -6;
    }
    int mem2disk(unsigned int start_addr, unsigned int size, char *name_in_disk) {
    unsigned char *content = (unsigned char *)start_addr;
    if (content == 0 || size == 0) return -10;
    return sys_create_in_root_large(name_in_disk, content, size);
    }