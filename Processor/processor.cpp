#include <stdio.h>

#include "processor.h"
#include "..\Common\file_reading.h"
#include "..\Stack\stack.h"
#include "..\Stack\stack_logs.h"
#include "..\Stack\stack_verification.h"

void calculate(Command *commands, int amount_of_commands) {
    Stack stk = {};
    StackCtr(&stk, 0);

    int n_command = 0;

    while (n_command < amount_of_commands) {
        if (commands[n_command].cmd == HLT_CMD) {
            break;
        }

        int first_popped = 0;

        switch (commands[n_command].cmd) {
            case PUSH_CMD:
                StackPush(&stk, commands[n_command].val);
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
