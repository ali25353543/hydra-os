#include <shell.h>
#include <fb.h>
#include <keyboard.h>
#include <serial.h>
#include <snake.h>
#include <beep.h>
#include <types.h>
#include <users.h>
#include <tar.h>
#include <vars.h>

#define COMMAND_BUFFER_SIZE 256

static char command_buffer[COMMAND_BUFFER_SIZE];
static unsigned int buffer_index = 0;
static char *prompt = NULL;
static int state = 0;
static uint8_t flags = 0;
static char commands[256][32] = {
    "help", "clear", "echo", "about",
    "play", "beep", "set", "get",
    "unset", "logout", "beep", "dir",
    "ls", "cat", "touch", "rawcopy",
    "edit", "set", "get", "unset",
    "tty"
};


/** shell_clear_command:
 *  Clears the screen
 */
void shell_clear_command(void)
{
    fb_clear();
}

/** shell_help_command:
 *  Displays help information
 */
void shell_help_command()
{
    fb_puts("Available commands:\n");
    fb_puts("  help  - Display this help message\n");
    fb_puts("  clear - Clear the screen\n");
    fb_puts("  echo  - Echo back your text\n");
    fb_puts("  about - Display OS information\n");
    fb_puts("  play  - Play Snake game!\n");
    fb_puts("  beep  - Make Sound on buzzer\n");
    fb_puts("  set   - Set an Variable with Value\n");
    fb_puts("  get   - Get and Print Variable's Value\n");
    fb_puts("  unset - Unset an Variable\n");
    fb_puts("  dir   - List files in DOS mode\n");
    fb_puts("  ls    - List files in UNIX mode\n");
    fb_puts("  touch - Create file in TARFS\n");
    fb_puts("  cat   - Show content of file in TARFS\n");
    fb_puts("  tty   - Switch Current Frame Buffer page\n");

}

/** shell_echo_command:
 *  Echoes back the command arguments
 *
 *  @param args  The arguments to echo
 */
void shell_echo_command(char *args)
{
    fb_puts(args);
    fb_puts("\n");
}

/** shell_about_command:
 *  Displays information about the OS
 */
int shell_about_command(void)
{
    fb_puts("Hydra OS - Chapter 1-6 Implementation\n");
    fb_puts("A simple operating system kernel\n");
    fb_puts("Built following 'The little book about OS development'\n");
    fb_puts("Developer - MultiX\n");
    fb_puts("Developer Message - \n"
        "as a full-stack developer i always use high-level programming languages like javascript, dart, golang, and others\n"
        "\n"
        "i really want to challenge myself and go through a new journey to deal with low-level code\n"
        "it's really funny, hard, with a lot of tears tbh, but it was a really fun experience\n");
    return 0;
}

/** shell_play_command:
 *  Launches the Snake game
 */
int shell_play_command(void)
{
    state = snake_game();
    /* After game ends, redraw shell */
    fb_clear();
    fb_puts("Welcome back to Hydra OS!\n");
    fb_puts("Type 'help' for available commands.\n\n");
    return state;
}

/** shell_execute_command:
 *  Executes a command
 */
int shell_execute_command(char *buf)
{
    /* Null-terminate the command */
    buf[buffer_index] = '\0';
    /* Skip empty commands */
    if (buffer_index == 0) {
        fb_puts(prompt);
        return 0;
    }
    
    /* Parse command and arguments */
    char *args = buf; //Compability with old commands
    //char *cmd = buf;
    while (*args && *args != ' ')
    {
        args++;
    }
    if (*args == ' ')
    {
        *args = '\0';
        args++;
    }
    
    int argc = 0;
    char **argv = strsplit(buf, ' ');
    while (argv[argc][0] != 0)
    {
        argc++;
    }
    /* Execute command */
    if (strcmp(argv[0], commands[0]) == 0) {
        shell_help_command();
        buffer_index = 0;
        fb_puts(prompt);
    } else if (strcmp(argv[0], "clear") == 0) {
        shell_clear_command();
        buffer_index = 0;
        fb_puts(prompt);
    } else if (strcmp(argv[0], "echo") == 0) {
        shell_echo_command(args);
        buffer_index = 0;
        fb_puts(prompt);
    } else if (strcmp(argv[0], "about") == 0) {
        shell_about_command();
        buffer_index = 0;
        fb_puts(prompt);
    } else if (strcmp(argv[0], "play") == 0) {
        state = shell_play_command();
        buffer_index = 0;
        fb_puts(prompt);
    } else if (strcmp(argv[0], "logout") == 0) {
        fb_clear();
        users_init(tar_read("users.txt"));
        shell_init(login(), 1);
        buffer_index = 0;
    } else if (strcmp(argv[0], "beep") == 0) {
        char *freq = argv[1];
        char *duration = argv[2];
        int int_freq = str_to_int(freq);
        int int_duration = str_to_int(duration);
        beep(int_freq,int_duration);
        buffer_index = 0;
        fb_puts(prompt);
    } else if (strcmp(argv[0], "dir") == 0) {
        tar_dir();
        buffer_index = 0;
        fb_puts(prompt);
    } else if (strcmp(argv[0], "ls") == 0) {
        tar_ls();
        buffer_index = 0;
        fb_puts(prompt);
    } else if (strcmp(argv[0], "cat") == 0) {
        fb_puts(tar_read(args) ? tar_read(args) : "\n");
        buffer_index = 0;
        fb_puts(prompt);
    } else if (strcmp(argv[0], "touch") == 0) {
        tar_create(args, 0) ? fb_puts("File created successfully.\n") : fb_puts("\n");
        buffer_index = 0;
        fb_puts(prompt);
    } else if (strcmp(argv[0], "exec") == 0) {
        exec(argv[1], args) ? fb_puts("File executed successfully.\n") : fb_puts("\n");
        buffer_index = 0;
        fb_puts(prompt);
    } else if (strcmp(argv[0], "edit") == 0) {
        int edit(char *file_name);
        edit(argv[1]);
        buffer_index = 0;
        fb_puts(prompt);
    } else if (strcmp(argv[0], "set") == 0) {
        if (argv[1][0] == 0) {
            var_ls();
        } else {
            char **parts = strsplit(argv[1], '=');
            set(parts[0], str_to_int(parts[1]));
        }
        buffer_index = 0;
        fb_puts(prompt);
    } else if (strcmp(argv[0], "get") == 0) {
        fb_puts(int_to_str(get(argv[1])));
        fb_putc('\n');
        buffer_index = 0;
        fb_puts(prompt);
    } else if (strcmp(argv[0], "unset") == 0) {
        unset(argv[1]);
        buffer_index = 0;
        fb_puts(prompt);
    } else if (strcmp(argv[0], "tty") == 0) {
        if (str_to_int(argv[1]) < 0 || str_to_int(argv[1]) > 7)
        {
            fb_puts("Invailid Page number.\n");
        } else {
            unsigned short select_cursor();
            void set_cursor_value(unsigned short value);
            unsigned short pos = select_cursor();
            pos -= 80;
            set_cursor_value(pos);
            fb_switch_page(str_to_int(argv[1]));
            extern char *fb;
            if (fb[0] == 0)
            {
                fb_clear();
            }
        }
        buffer_index = 0;
        fb_puts(prompt);
    } else if (strcmp(argv[1], ">") == 0) {
        tar_write(argv[0], argv[2], strlen(argv[2])) ? fb_puts("File written successfully.\n") : fb_puts("\n");
        buffer_index = 0;
        fb_puts(prompt);
    } else {
        /* we’ll never get here, unless the module code returns */
        fb_puts("Unknown command: ");
        fb_puts(argv[0]);
        fb_puts("\nType 'help' for available commands.\n");
        buffer_index = 0;
        fb_puts(prompt);
        return 127;
    }
    /* Reset buffer and show prompt */
    return 0;
}

/** shell_init:
 *  Initializes the shell with Shell Flags (SF)
 *  Shell Flags:
 *  ___________________________________________
 *  | bit | Description                       |
 *  |  0  | Login Shell Flag (LSF)            |
 *  |  1  | (0: UNIX, 1: DOS)                 |
 *  |  2  | Change Prompt Ability Flag (CPAF) |
 *  |  3  | Variable Table Flag (VTF)         |
 *  |  4  | AM8 Flag (AM8F)                   |
 *  | 5-7 | Unuused (Yet)                     |
 */
void shell_init(char *username, uint8_t shell_flags)
{
    flags = shell_flags;
    buffer_index = 0;
    prompt = username;
    state = 0;
    prompt[strlen(username)] = '>';
    prompt[strlen(username) + 1] = ' ';
    fb_puts("Welcome to Hydra OS!\n");
    fb_puts("Type 'help' for available commands.\n\n");
    fb_puts(prompt);
}

/** shell_update:
 *  Updates the shell (call this in main loop)
 */
void shell_update(void)
{
    char c = keyboard_get_char();
    
    if (c != 0) {
        serial_write("GOT CHAR: ");
        serial_write(&c);
        serial_write("\n");
        
        if (c == '\n') {
            /* Execute command */
            fb_putc('\n');
            set("$", shell_execute_command(command_buffer));
        } else if (c == '\b') {
            /* Handle backspace */
            if (buffer_index > 0) {
                buffer_index--;
                fb_putc('\b');
            } else {
                beep(480, 100);
            }
        } else if (c == 27) {
            fb_clear();
            users_init(tar_read("users.txt"));
            shell_init(login(), 1);
        } else if (buffer_index < COMMAND_BUFFER_SIZE - 1) {
            /* Add character to buffer */
            command_buffer[buffer_index] = c;
            buffer_index++;
            fb_putc(c);
        }
    }
}
