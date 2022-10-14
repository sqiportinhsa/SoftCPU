#include <stdio.h>
#include <string.h>

#include "processor.h"
#include "..\Common\file_reading.h"
#include "..\Stack\stack.h"
#include "..\Stack\stack_logs.h"
#include "..\Stack\stack_verification.h"

void calculate(CPU *cpu) {
    int n_command = 0;

    while (n_command < cpu->amount_of_cmds && cpu->ip < cpu->code_len) {
        int cmd = 0;
        cpu->ip += get_val(&(cpu->code[cpu->ip]), &cmd);
        ++cpu->ip;

        if (cmd == HLT_CMD) {
            break;
        }

        int first_popped = 0;
        int cmd_val      = 0;

        switch (cmd) {
            case PUSH_CMD:
                cpu->ip += get_val(&(cpu->code[cpu->ip]), &cmd_val);
                ++cpu->ip;
                StackPush(cpu->cpu_stack, cmd_val);
                break;
            case ADD_CMD:
                StackPush(cpu->cpu_stack, StackPop(cpu->cpu_stack) + StackPop(cpu->cpu_stack));
                break;
            case SUB_CMD:
                first_popped = StackPop(cpu->cpu_stack);
                StackPush(cpu->cpu_stack, StackPop(cpu->cpu_stack) - first_popped  );
                break;
            case MUL_CMD:
                StackPush(cpu->cpu_stack, StackPop(cpu->cpu_stack) * StackPop(cpu->cpu_stack));
                break;
            case DIV_CMD:
                first_popped = StackPop(cpu->cpu_stack);
                StackPush(cpu->cpu_stack, StackPop(cpu->cpu_stack) / first_popped  );
                break;
            case OUT_CMD:
                printf("result: %d", StackPop(cpu->cpu_stack));
                break;
            default:
                break;
        }

        ++n_command;
    }
}

void real_CPU_constructor(CPU *cpu, size_t code_len, int line, const char* func, const char* file) {
    cpu->cpu_stack = nullptr;
    cpu->logs      = nullptr;
    cpu->code      = nullptr;

    cpu->cpu_stack = (Stack*) calloc(1, sizeof(Stack));
    if (cpu->cpu_stack == nullptr) {
        printf("error: not enought memory for stack in CPU constructor\n");
        free(cpu->code);
        free(cpu->cpu_stack);
        free(cpu->logs);
        return;
    }

    StackCtr(cpu->cpu_stack, 0);

    cpu->code = (char*) calloc(code_len, sizeof(char));
    if (cpu->code == nullptr) {
        printf("error: not enougth memory for code in CPU constructor\n");
        free(cpu->code);
        free(cpu->cpu_stack);
        free(cpu->logs);
        return;
    }

    cpu->logs = (CPU_logs*) calloc(1, sizeof(CPU_logs));
    if (cpu->logs == nullptr) {
        printf("error: not enougth memory for logs in CPU constructor\n");
        free(cpu->code);
        free(cpu->cpu_stack);
        free(cpu->logs);
        return;
    }

    cpu->logs->line_of_creation = line;
    cpu->logs->file_of_creation = file;
    cpu->logs->func_of_creation = func;

    cpu->code_len = code_len;
}

void CPU_destructor(CPU *cpu) {
    free(cpu->code);
    free(cpu->cpu_stack);

    cpu->amount_of_cmds = 0;
    cpu->code_len       = 0;
    cpu->ass_version    = 0;
    cpu->ip             = 0;
}
