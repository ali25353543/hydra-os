#ifndef FAT32_H
#define FAT32_H

// الهيكل الممتد لنظام FAT32 (يبدأ من البايت 36 تماماً)
typedef struct fat_extBS_32 {
    unsigned int   table_size_32;       // 4 بايت
    unsigned short extended_flags;      // 2 بايت
    unsigned short fat_version;         // 2 بايت
    unsigned int   root_cluster;        // 4 بايت
    unsigned short fat_info;            // 2 بايت
    unsigned short backup_BS_sector;    // 2 بايت
    unsigned char  reserved_0[12];      // ⚠️ مصفوفة صريحة 12 بايت (منعاً لتخريب الإزاحة)
    unsigned char  drive_number;        // 1 بايت
    unsigned char  reserved_1;          // 1 بايت
    unsigned char  boot_signature;      // 1 byte (0x29)
    unsigned int   volume_id;           // 4 بايت
    unsigned char  volume_label[11];    // ⚠️ مصفوفة صريحة 11 بايت لاسم القرص
    unsigned char  fat_type_label[8];   // ⚠️ مصفوفة صريحة 8 بايت وتساوي "FAT32   "
} __attribute__((packed)) fat_extBS_32_t;

// الهيكل الرئيسي لقطاع الإقلاع (BIOS Parameter Block)
typedef struct fat_BS {
    unsigned char  bootjmp[3];          // ⚠️ مصفوفة صريحة 3 بايت
    unsigned char  oem_name[8];         // ⚠️ مصفوفة صريحة 8 بايت
    unsigned short bytes_per_sector;    // 2 بايت
    unsigned char  sectors_per_cluster; // 1 بايت
    unsigned short reserved_sector_count;// 2 بايت
    unsigned char  table_count;         // 1 بايت (غالباً 2)
    unsigned short root_entry_count;    // 2 بايت (0 في FAT32)
    unsigned short total_sectors_16;    // 2 بايت (0 في FAT32)
    unsigned char  media_type;          // 1 بايت
    unsigned short table_size_16;       // 2 بايت (0 في FAT32)
    unsigned short sectors_per_track;   // 2 بايت
    unsigned short head_side_count;     // 2 بايت
    unsigned int   hidden_sector_count; // 4 بايت
    unsigned int   total_sectors_32;    // 4 بايت
    
    // يدمج هنا القسم الممتد عند الإزاحة الدقيقة 36 بايت
    fat_extBS_32_t ext; 
} __attribute__((packed)) fat_BS_t;

// هيكل إدخال الملف القياسي (Directory Entry) بحجم 32 بايت تماماً
typedef struct fat_dir_entry {
    unsigned char  name[11];            // ⚠️ مصفوفة صريحة 11 بايت (8 للاسم + 3 للامتداد)
    unsigned char  attr;                // 1 بايت (0x20 للأرشفة)
    unsigned char  nt_res;              // 1 بايت
    unsigned char  crt_time_ten_ms;     // 1 بايت
    unsigned short crt_time;            // 2 بايت
    unsigned short crt_date;            // 2 bytes
    unsigned short lst_acc_date;        // 2 بايت
    unsigned short first_cluster_hi;    // 2 بايت
    unsigned short wrt_time;            // 2 بايت
    unsigned short wrt_date;            // 2 بايت
    unsigned short first_cluster_lo;    // 2 بايت
    unsigned int   file_size;           // 4 بايت
} __attribute__((packed)) fat_dir_entry_t;

void init_fat32_file_system(unsigned int partition_lba_start);
int sys_create_in_root(const char *linux_path, const unsigned char *data, unsigned int size);
int mem2disk(unsigned int start_addr, unsigned int size, char *name_in_disk);

#endif
