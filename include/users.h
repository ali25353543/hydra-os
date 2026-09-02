#ifndef INCLUDE_USERS_H
#define INCLUDE_USERS_H

#include <types.h>

#define MAX_USERS 64
#define MAX_USER_NAME_LENGTH 64
#define MAX_USER_PASSWORD_LENGTH 64

typedef struct {
    char name[MAX_USER_NAME_LENGTH];
    char password[MAX_USER_PASSWORD_LENGTH];
} user_t;

void users_init(
    /*char *file*/);
uint8_t login();

#endif