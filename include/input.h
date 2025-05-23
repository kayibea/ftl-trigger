#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

int input_open(void);
void input_close(void);
void input_poll(void);
bool input_should_trigger(void);

#endif
