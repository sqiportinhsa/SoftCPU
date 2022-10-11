#include <stdio.h>

#include "processor.h"
#include "..\Common\file_reading.h"
#include "..\Stack\stack.h"
#include "..\Stack\stack_logs.h"
#include "..\Stack\stack_verification.h"

void calculate(char *text, int amount_of_commands, size_t amount_of_symbols) {
    Stack stk = {};
    StackCtr(&stk, 0);

    int n_command = 0;
    size_t n_elem = 0;

    while (n_command < amount_of_commands && n_elem < amount_of_symbols) {
        int cmd = 0;
        n_elem += get_val(&(text[n_elem]), &cmd);
        ++n_elem;

        if (cmd == HLT_CMD) {
            break;
        }

        int first_popped = 0;
        int cmd_val      = 0;

        switch (cmd) {
            case PUSH_CMD:
                n_elem += get_val(&(text[n_elem]), &cmd_val);
                ++n_elem;
                StackPush(&stk, cmd_val);
                break;
            case ADD_CMD:
                StackPush(&stk, StackPop(&stk) + StackPop(&stk));
                break;
            case SUB_CMD:
                first_popped = StackPop(&stk);
                StackPush(&stk, StackPop(&stk) - first_popped  );
                break;
            case MUL_CMD:
                StackPush(&stk, StackPop(&stk) * StackPop(&stk));
                break;
            case DIV_CMD:
                first_popped = StackPop(&stk);
                StackPush(&stk, StackPop(&stk) / first_popped  );
                break;
            case OUT_CMD:
                printf("result: %d", StackPop(&stk));
                break;
            default:
                break;
        }

        ++n_command;
    }

    StackDestr(&stk);
}
