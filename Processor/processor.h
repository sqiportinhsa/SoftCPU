#ifndef PROCESSOR
#define PROCESSOR

#include "..\common.h"

#define PROC_VER 1

typedef struct {
    Commands cmd;
    int      val;
} Command;

int get_commands(char *text, Command *commands, size_t amount_of_elems);
Commands get_cmd(int val);
void calculate(Command *commands, int amount_of_commands);

#endif