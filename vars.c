#include <types.h>
#include <vars.h>
#include <fb.h>

vars_t vars[64];

int set(char *var_name, int value)
{
    unsigned char i = 0;
    while (vars[i].name[0] != 0)
    {
        if (strcmp(vars[i].name, var_name) == 0) {
            vars[i].value = value;
            return 0;
        }
        i++;
    }
    strcopy(var_name, vars[i].name);
    vars[i].value = value;
    return 0;
}

int get(char *var_name)
{
    unsigned char i = 0;
    while (vars[i].name[0] != 0)
    {
        if (strcmp(vars[i].name, var_name) == 0) {
            return vars[i].value;
        }
    }
    return 0;
}

void var_ls()
{
    unsigned char i = 0;
    while (vars[i].name[0] != 0)
    {
        fb_puts(vars[i].name);
        fb_putc('=');
        fb_puts(int_to_str(vars[i].value));
        fb_putc('\n');
        i++;
    }
}

int unset(char *var_name)
{

    unsigned char i = 0;
    while (vars[i].name[0] != 0)
    {
        if (strcmp(vars[i].name, var_name) == 0) {
            for (uint8_t j = 0; i < 32; i++)
            {
                vars[i].name[j] = 0;
            }
            vars[i].value = 0;
            return 0;
        }
    }
    return 0;
}