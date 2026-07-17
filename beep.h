#ifndef INCLUDE_BEEP_H
#define INCLUDE_BEEP_H

void beep_init(void);
void beep_start(int frequency);
void beep_stop(void);
void beep(int frequency, int duration_ms);

#endif