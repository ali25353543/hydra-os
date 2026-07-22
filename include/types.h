#ifndef INCLUDE_TYPES_H
#define INCLUDE_TYPES_H

typedef unsigned char uint8_t;
typedef char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned long uint64_t;
typedef long int64_t;
typedef unsigned int size_t;

int strcmp(const char *s1, const char *s2);
char **strsplit(const char *str, char delim);
int strlen(const char *s1);
int strncmp(const char *s1, const char *s2, unsigned int n);

#endif //INCLUDE_TYPES_H