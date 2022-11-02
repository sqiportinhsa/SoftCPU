#ifndef UTILS
#define UTILS

#include "common.hpp"

typedef struct {
    Commands cmd;
    int      val;
} Command;

int get_commands(char *text, Command *commands, size_t amount_of_elems);
Commands get_cmd(int val);

#endif