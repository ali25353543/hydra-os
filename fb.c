#include <types.h>
#include <io.h>
#include <fb.h>

unsigned char active_page = 0;

/* The framebuffer address */
char *fb = (char *) (0x000B8000);

/* Screen dimensions */
#define FB_WIDTH 80
#define FB_HEIGHT 25

/* Cursor positions */
unsigned short cursor_pos_0 = 0;
unsigned short cursor_pos_1 = 0;
unsigned short cursor_pos_2 = 0;
unsigned short cursor_pos_3 = 0;
unsigned short cursor_pos_4 = 0;
unsigned short cursor_pos_5 = 0;
unsigned short cursor_pos_6 = 0;
unsigned short cursor_pos_7 = 0;

unsigned short select_cursor()
{
    switch (active_page)
    {
    case 0:
        return cursor_pos_0;
        break;
    case 1:
        return cursor_pos_1;
        break;
    case 2:
        return cursor_pos_2;
        break;
    case 3:
        return cursor_pos_3;
        break;
    case 4:
        return cursor_pos_4;
        break;
    case 5:
        return cursor_pos_5;
        break;
    case 6:
        return cursor_pos_6;
        break;
    case 7:
        return cursor_pos_7;
        break;
    default:
        break;
    }
    return 0;
}

void set_cursor_value(unsigned short value)
{
    switch (active_page)
    {
    case 0:
        cursor_pos_0 = value;
        break;
    case 1:
        cursor_pos_1 = value;
        break;
    case 2:
        cursor_pos_2 = value;
        break;
    case 3:
        cursor_pos_3 = value;
        break;
    case 4:
        cursor_pos_4 = value;
        break;
    case 5:
        cursor_pos_5 = value;
        break;
    case 6:
        cursor_pos_6 = value;
        break;
    case 7:
        cursor_pos_7 = value;
        break;
    default:
        break;
    }
}

/** fb_move_cursor:
 *  Moves the cursor of the framebuffer to the given position
 *
 *  @param pos The new position of the cursor
 */
void fb_move_cursor(unsigned short pos)
{
    pos += (0xFA0 * active_page) / 2;
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT, pos & 0x00FF);
}

/** fb_write_cell:
 *  Writes a character with the given foreground and background to position i
 *  in the framebuffer.
 *
 *  @param i  The location in the framebuffer
 *  @param c  The character
 *  @param fg The foreground color
 *  @param bg The background color
 */
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
    fb[i] = c;
    fb[i + 1] = ((bg & 0x0F) << 4) | (fg & 0x0F);  // ← CORRECT
}
/** fb_clear:
 *  Clears the screen.
 */
void fb_clear(void)
{
    unsigned int i;
    for (i = 0; i < FB_WIDTH * FB_HEIGHT; i++) {
        fb_write_cell(i * 2, ' ', FB_WHITE, FB_BLACK);
    }
    unsigned short pos = select_cursor();
    pos = 0;
    fb_move_cursor(pos);
    set_cursor_value(pos);
}

/** fb_scroll:
 *  Scrolls the screen up by one line.
 */
void fb_scroll(void)
{
    unsigned int i;
    
    /* Move all lines up by one */
    for (i = 0; i < (FB_HEIGHT - 1) * FB_WIDTH; i++) {
        fb[i * 2] = fb[(i + FB_WIDTH) * 2];
        fb[i * 2 + 1] = fb[(i + FB_WIDTH) * 2 + 1];
    }
    
    /* Clear the last line */
    for (i = (FB_HEIGHT - 1) * FB_WIDTH; i < FB_HEIGHT * FB_WIDTH; i++) {
        fb_write_cell(i * 2, ' ', FB_WHITE, FB_BLACK);
    }

    set_cursor_value((FB_HEIGHT - 1) * FB_WIDTH);
}

/** fb_putc:
 *  Writes a character to the screen with newline handling.
 *
 *  @param c  The character to write
 */
void fb_putc(char c)
{
    unsigned short pos = 0;
    if (c == '\n') {
        /* Move to next line */
        pos = select_cursor();

        pos = (pos / FB_WIDTH + 1) * FB_WIDTH;

        set_cursor_value(pos);
    } else if (c == '\b') {
        /* Backspace */
        if (select_cursor() > 0) {
            pos = select_cursor();
            pos--;
            set_cursor_value(pos);
            fb_write_cell(pos * 2, ' ', FB_WHITE, FB_BLACK);
        }
    } else if (c == '\t') {
        /* Tab - move to next multiple of 8 */
            pos = select_cursor();
            pos = (pos + 8) & ~7;
            set_cursor_value(pos);
    } else {
        /* Regular character */
        pos = select_cursor();
        fb_write_cell(pos * 2, c, FB_WHITE, FB_BLACK);
        pos++;
        set_cursor_value(pos);
    }
    pos = select_cursor();
    /* Scroll if needed */
    if (pos >= FB_WIDTH * FB_HEIGHT) {
        fb_scroll();
    }
    
    fb_move_cursor(pos);
}

/** fb_puts:
 *  Writes a null-terminated string to the screen.
 *
 *  @param str  The string to write
 */
void fb_puts(char *str)
{
    unsigned int i = 0;
    while (str[i] != '\0') {
        fb_putc(str[i]);
        i++;
    }
}

/** fb_write_char:
 *  Writes a single character to the screen.
 *
 *  @param c  The character to write
 */
void fb_write_char(char c)
{
    fb_putc(c);
}

/** fb_write:
 *  Writes the contents of the buffer buf of length len to the screen.
 *
 *  @param buf  The buffer to write
 *  @param len  The length of the buffer
 *  @return     The number of characters written
 */
int fb_write(char *buf, unsigned int len)
{
    unsigned int i;
    for (i = 0; i < len; i++) {
        fb_putc(buf[i]);
    }
    return len;
}

void fb_switch_page(unsigned char page_num)
{
    unsigned short offset = (page_num * 0xFA0) / 2;
    outb(FB_COMMAND_PORT, 0x0C);
    outb(FB_DATA_PORT, (offset >> 8) & 0xFF);

    outb(FB_COMMAND_PORT, 0x0D);
    outb(FB_DATA_PORT, offset & 0xFF);
    if (page_num < active_page)
    {
        fb -= (int)(char *) ((active_page - page_num) * 0xFA0);
    } else if (page_num > active_page)
    {
        fb += (int)(char *) ((page_num - active_page) * 0xFA0);
    }

    //unsigned short total_offset = (offset + 0);//strlen(fb));
    fb_move_cursor(select_cursor());
    active_page = page_num;
}
