//#include ""

/** strcmp:
 *  Simple string comparison function
 *
 *  @param s1  First string
 *  @param s2  Second string
 *  @return    0 if equal, non-zero otherwise
 */
int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

#define MAX_PARTS 100
#define MAX_LEN 100

char *g_parts[MAX_PARTS];
char g_buffer[MAX_PARTS][MAX_LEN];  // مخزن مؤقت للنسخ

char **strsplit(const char *str, char delim)
{
    int part_count = 0;
    int char_count = 0;
    
    while (*str && part_count < MAX_PARTS - 1) {
        // تجاوز المحددات المتتالية
        while (*str == delim) str++;
        if (!*str) break;
        
        // نسخ الجزء الحالي
        char_count = 0;
        while (*str && *str != delim && char_count < MAX_LEN - 1) {
            g_buffer[part_count][char_count++] = *str++;
        }
        g_buffer[part_count][char_count] = '\0';
        g_parts[part_count] = g_buffer[part_count];
        part_count++;
    }
    
    g_parts[part_count] = 0;
    return g_parts;
}

int strlen(const char *s1)
{
    unsigned int i = 0;
    while (*s1)
    {
        s1++;
        i++;
    }
    
    return i;
}

int strncmp(const char *s1, const char *s2, unsigned int n)
{
    if (n == 0) return 0;
    
    while (n > 0) {
        if (*s1 != *s2) {
            return *(unsigned char *)s1 - *(unsigned char *)s2;
        }
        if (*s1 == '\0') {
            return 0;  // وصلنا إلى نهاية السلسلة
        }
        s1++;
        s2++;
        n--;
    }
    return 0;  // جميع الأحرف متطابقة
}