#ifndef INTERPRETATOR
#define INTERPRETATOR

#include "..\Stack\stack.h"
#include "..\Stack\stack_logs.h"
#include "..\Stack\stack_verification.h"

typedef struct {
    char* cmd_ptr;
    char* val_ptr;
    int   cmd_len;
} Command;

void calculate(Command* commands, int amount_of_strings);
void do_command(Stack *stk, Command* commands, int ncommand);

#endif