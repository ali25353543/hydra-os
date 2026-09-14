#include <types.h>
#include <vars.h>

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