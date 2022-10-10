#ifndef PROCESSOR
#define PROCESSOR

#include "..\common.h"

typedef struct {
    Commands cmd;
    int      val;
} Command;

void get_commands(char *text, Command *commands, int amount_of_commands, size_t amount_of_elems);
Commands get_cmd(int val);
void calculate(Command *commands, int amount_of_commands);

#endif