#include <fb.h>
int psfedit(char *psf_file) {
    fb_clear();
    fb_puts(psf_file);
    fb_clear();
    fb_puts("Alintosh PSF Editor\n");
    return 0;
}