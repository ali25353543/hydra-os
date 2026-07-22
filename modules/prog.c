#include "../fb.h"
#include "../serial.h"

typedef int (*serial_write_t)(char *);
typedef void (*fb_puts_t)(char *);

int module(serial_write_t kernel_serial_write, fb_puts_t kernel_fb_puts) {
    if (kernel_fb_puts) {
        kernel_fb_puts("hello from module via kernel!\n");
    }
    if (kernel_serial_write) {
        kernel_serial_write("This is simple driver\n");
    }
    return 0;
}
