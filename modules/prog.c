#include "../fb.h"
#include "../serial.h"
int main() {
	fb_puts("hello\n");
	serial_write("This is simple driver\n");
	return 0;
}