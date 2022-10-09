#include <string.h>

#include "..\common.h"
#include "assemblr.h"

void calculate(Command* commands, int amount_of_strings) {
    int ncommand = 0;

    Stack stk = {};
    StackCtr(&stk, 0);

    while (stricmp(commands[ncommand].cmd_ptr, "HLT") != 0 && ncommand < amount_of_strings) {
        do_command(&stk, commands, ncommand);
        ++ncommand;
    }
}

void do_command(Stack *stk, Command* commands, int ncommand) {
    int len = commands[ncommand].cmd_len;

    char cmd[5] = {};
    memcpy(cmd, commands[ncommand].cmd_ptr, len);
    cmd[len] = '\0';

    if (stricmp("PUSH", cmd) == 0) {
        if (commands[ncommand].val_ptr == nullptr) {
            printf("incorrect push: value expected\n");
            return;
        }
        StackPush(stk, *commands[ncommand].val_ptr);

    } else if (stricmp("ADD", cmd) == 0) {
        int a = StackPop(stk);
        int b = StackPop(stk);

        StackPush(stk, a + b);

    } else if (stricmp("SUB", cmd) == 0) {
        int a = StackPop(stk);
        int b = StackPop(stk);

        StackPush(stk, b - a);

    } else if (stricmp("MUL", cmd) == 0) {
        int a = StackPop(stk);
        int b = StackPop(stk);

        StackPush(stk, a * b);

    } else if (stricmp("DIV", cmd) == 0) {
        int a = StackPop(stk);
        int b = StackPop(stk);

        StackPush(stk, b / a);

    } else if (stricmp("OUT", cmd) == 0) {
        printf(PFORMAT, StackPop(stk));
    } else if (stricmp("HLT", cmd) == 0) {
        return;
    } else {
        printf("invalid command\n");
    }
}
