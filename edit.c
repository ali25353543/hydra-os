#include <fb.h>
#include <tar.h>
#include <types.h>
int edit(char *file_name)
{
    fb_clear();
    char *file = tar_read(file_name);
    fb_puts("Alintosh PSF Editor\n");
    fb_puts(file);
    return 0;
}