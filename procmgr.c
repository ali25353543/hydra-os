#include <types.h>
typedef struct {
    unsigned char name[32];
    unsigned char pid;
    unsigned int addr;
    unsigned short pos;
    unsigned int addr_pos;
} proc_t;

proc_t procs[64];

uint8_t proc_exists(unsigned char pid)
{
    unsigned char i = 0;
    while (i != pid)
    {
        if (i == 64)
        {
            return 0; //Proc. does not exist
        }
        i++;
    }
    procs[i].pid = pid;
    i = 0;
    return 1; //Proc. exists
}

/**
    add_proc - Adds Proccess in ProcTable
    @return 0 - Added Proc. Successfully
    @return 1 - ProcTable is full
*/

int add_proc(unsigned int addr)
{
    unsigned char i = 0;
    while (procs[i].pid != 0)
    {
        if (i == 64)
        {
            return 1; //ProcTable is full
        }
        i++;
        procs[i].pid = i;
    }
    procs[i].addr = addr;
    return 0; //Successfully added Proc.
}
unsigned int get_proc_addr(unsigned char pid)
{
    if (proc_exists(pid))
    {
        return procs[pid].addr;
    } else {
        return 0xFFFFFFFF;
    }
}
int exec_proc(unsigned char pid)
{
    if (proc_exists(pid)) {
        defcall_type(void, call_t, void)
        if (procs[pid].addr > 0x00010000) procs[pid].addr = 0x00010000;
        call_t func = (call_t)procs[pid].addr;
        func();
    }
    return 0;
}
int set_proc_name(unsigned char pid, char *name)
{
    if (proc_exists(pid))
    {
        strcopy(name, (char *)procs[pid].name);
        return 0;
    }
    return 1;
}
char *get_proc_name(unsigned char pid)
{
    if (proc_exists(pid))
    {
        return (char *)procs[pid].name;
    }
    return 0;
}
uint8_t get_proc_id(char *name)
{
    unsigned char i = 0;
    while (strcmp((char *)procs[i].name, name) != 0)
    {
        if (i == 64)
        {
            return 128;
        }
        i++;
    }
    return i;
}