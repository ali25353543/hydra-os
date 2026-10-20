typedef struct {
    char name[8];
    char typeflag;
    int sum_of_size;
    char linkname[3];
} __attribute__((packed)) alifs_header_t;

typedef struct {
    int actual_part_size;
    int used_size;
    int linkname;
    int reserved;
    alifs_header_t files_and_dirs[31];
} __attribute__((packed)) alifs_root_sector_header_t;

void alifs_init(char *alifs_start);

void alifs_dir();
/*
void alifs_ls();

char *alifs_read(char *file_name);

int alifs_create(char *file_name, unsigned int alloc_size);

int alifs_write(char *file_name, char *data, unsigned int data_size);

int exec(char *file_name);
*/