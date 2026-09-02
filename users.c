#include <users.h>
#include <keyboard.h>
#include <fb.h>
user_t users[MAX_USERS];

void users_init(char *file) {
    // تهيئة جميع المستخدمين
    for (uint8_t i = 0; i < MAX_USERS; i++) {
        users[i].name[0] = '\0';
        users[i].password[0] = '\0';
    }
   char *token = "";
   char **tokens = strsplit(file, '\n');
   int i = 0;
   while (*tokens != 0 && i < MAX_USERS)
   {
    token = *tokens + 5;
    strcopy(token, users[i].name);
    tokens++;
    token = *tokens + 9;
    strcopy(token, users[i].password);
    tokens++;
    i++;

   }
}
static uint8_t check_name(char *name) {
    for (uint8_t i = 0; i < MAX_USERS; i++) {
        if (strcmp(name, users[i].name) == 0) {
            return i; // إرجاع الفهرس بدلاً من 0
        }
    }
    return 255; // لم يتم العثور
}

static uint8_t check_password(char *password) {
    for (uint8_t i = 0; i < MAX_USERS; i++) {
        if (strcmp(password, users[i].password) == 0) {
            return i; // إرجاع الفهرس بدلاً من 0
        }
    }
    return 255; // لم يتم العثور
}

uint8_t login() {
    char name[MAX_USER_NAME_LENGTH];
    char password[MAX_USER_PASSWORD_LENGTH];
    uint8_t i = 0;
    uint8_t c = 0;
    
    fb_puts("User name: ");
    
    // قراءة اسم المستخدم
    i = 0;
    while (i < MAX_USER_NAME_LENGTH - 1) {
        c = keyboard_get_char();
        if (c == '\n') break;
        if (c != 0) {
            fb_putc(c);
            name[i++] = c;
        }
    }
    name[i] = '\0';
    fb_putc('\n');
    
    // التحقق من وجود المستخدم
    uint8_t user_index = check_name(name);
    
    if (user_index == 255) {
        fb_puts("User not found!\n");
        return login();
    }
    
    fb_puts("Password: ");
    i = 0;
    while (i < MAX_USER_PASSWORD_LENGTH - 1) {
        c = keyboard_get_char();
        if (c == '\n') break;
        if (c != 0) {
            fb_putc('*'); // إخفاء كلمة المرور
            password[i++] = c;
        }
    }
    password[i] = '\0';
    fb_putc('\n');
    
    // التحقق من كلمة المرور
    if (check_password(password) == 0) {
        fb_puts("Login successful!\n");
        return 0;
    } else {
        fb_puts("Wrong password!\n");
        return login();
    }
   return 0;
}
