typedef struct 
{
    char name[32];
    int value;
} vars_t;

int set(char *var_name, int value);

int get(char *var_name);

void var_ls();

int unset(char *var_name);