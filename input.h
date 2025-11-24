#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdbool.h>

int input_number();
void input_str(const char *msg, char *buf, int size);
bool simple_yesno(const char *msg);

#endif