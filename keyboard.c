#include <keyboard.h>
#include <serial.h>

/* US QWERTY keyboard layout scan code to ASCII table */
static char scan_code_to_ascii[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ' 
};

static char scan_code_with_shift_to_ascii[] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' '
};

static char last_char = 0;
static char shift = 0;
/** keyboard_init:
 *  Initializes the keyboard driver
 */
void keyboard_init(void)
{
    last_char = 0;
}

/** keyboard_handle_interrupt:
 *  Handles a keyboard interrupt and converts scan code to ASCII
 *  
 *  @param scan_code The scan code from the keyboard
 */
void keyboard_handle_interrupt(unsigned char scan_code)
{
    // 1. معالجة حالة الضغط (Make Codes) - أقل من 0x80
    if (scan_code < 0x80) {
        // التحقق من أزرار Shift عند الضغط (Left Shift = 0x2A, Right Shift = 0x36)
        if (scan_code == 0x2A || scan_code == 0x36) {
            shift = 1;
            serial_write("Shift Pressed!\n");
            return; // نخرج فوراً، زر الـ Shift مستقل ولا يولد حرفاً بنفسه
        }

        // معالجة باقي الأزرار العادية
        if (scan_code < sizeof(scan_code_to_ascii)) {
            if (shift) {
                last_char = scan_code_with_shift_to_ascii[scan_code];
            } else {
                last_char = scan_code_to_ascii[scan_code];
            }
            
            // طباعة الحرف إذا لم يكن صفراً (الأزرار غير المعرفة مبرمجة كـ 0)
            if (last_char != 0) {
                serial_write("ASCII: ");
                serial_write(&last_char);
                serial_write("\n");
            }
        }
    } 
    // 2. معالجة حالة الإفلات (Break Codes) - أكبر من أو تساوي 0x80
    else {
        // التحقق من أزرار Shift عند الإفلات (Left Shift Break = 0xAA, Right Shift Break = 0xB6)
        if (scan_code == 0xAA || scan_code == 0xB6) {
            shift = 0;
            serial_write("Shift Released!\n");
        }
    }
}

/** keyboard_get_char:
 *  Gets the last character typed (non-blocking)
 *  
 *  @return The character, or 0 if no character available
 */
char keyboard_get_char(void)
{
    char c = last_char;
    last_char = 0;
    return c;
}