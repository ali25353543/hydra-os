typedef struct {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size_raw[12];
    char mtime[12];
    char chksum[8];
    char type[1];
    char linkname[100];
    char special_text[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char reserved[12];
} tar_header_t;

void tar_info(char *tar_file);

char *tar_read(char *tar_file, char *file_name);
