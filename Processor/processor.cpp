#include <stdio.h>

#include "processor.h"
#include "..\file_reading.h"
#include "..\Stack\stack.h"
#include "..\Stack\stack_logs.h"
#include "..\Stack\stack_verification.h"

void get_commands(char *text, Command *commands, size_t amount_of_elems) {
    int n_command = 0;
    size_t n_elem = 0;

    while (n_elem < amount_of_elems) {
        int cmd = 0;

        n_elem += get_val(&(text[n_elem]), &cmd);

        commands[n_command].cmd = get_cmd(cmd);

        ++n_elem;

        switch (commands[n_command].cmd) {
            case PUSH_CMD:
                n_elem += get_val(&(text[n_elem]), &(commands[n_command].val));
                ++n_elem;
                break;
            default:
                commands[n_command].val = 0;
        }
        ++n_command;
    }
}

Commands get_cmd(int val) {
    return (val > MIN_CMD && val < MAX_CMD) ? (Commands) val : NOT_A_CMD;
}

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
