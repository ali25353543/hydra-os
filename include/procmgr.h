#include <types.h>
uint8_t proc_exists(unsigned char pid);
int add_proc(unsigned int addr);
unsigned int get_proc_addr(unsigned char pid);
int exec_proc(unsigned char pid);
int set_proc_name(unsigned char pid, char *name);
char *get_proc_name(unsigned char pid);
uint8_t get_proc_id(char *name);
