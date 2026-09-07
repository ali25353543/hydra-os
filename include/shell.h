#ifndef INCLUDE_SHELL_H
#define INCLUDE_SHELL_H

/** shell_init:
 *  Initializes the shell
 */
void shell_init(char *username, char *tar_start);

/** shell_update:
 *  Updates the shell (call this in main loop)
 */
void shell_update(void);

int shell_execute_command(char *buf);

#endif /* INCLUDE_SHELL_H */