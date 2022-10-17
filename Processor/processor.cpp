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
        CPU_destructor(cpu);
        return;
    }

    StackCtr(cpu->cpu_stack, 0);

    cpu->code = (char*) calloc(code_len, sizeof(char));
    if (cpu->code == nullptr) {
        printf("error: not enougth memory for code in CPU constructor\n");
        CPU_destructor(cpu);
        return;
    }

    cpu->logs = (CPU_logs*) calloc(1, sizeof(CPU_logs));
    if (cpu->logs == nullptr) {
        printf("error: not enougth memory for logs in CPU constructor\n");
        CPU_destructor(cpu);
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
    free(cpu->logs);

    cpu->code       = nullptr;
    cpu->cpu_stack  = nullptr;
    cpu->logs       = nullptr;

    cpu->amount_of_cmds = 0;
    cpu->code_len       = 0;
    cpu->ass_version    = 0;
    cpu->ip             = 0;
}

void real_dump_cpu(CPU *cpu, FILE *logfile, const char *file, const char *func, int line) {
    if (logfile == nullptr) {
        printf("dump_cpu can't dump logs: null pointer to logfile\n");
        return;
    }

    fprintf(logfile, "logs called in %s at line %d, function %s:\n", file, line, func);

    if (cpu == nullptr) {
        fprintf(logfile, "can't dump cpu: null pointer to cpu\n");
        return;
    }
    
    if (cpu->logs == nullptr) {
        fprintf(logfile, "Information about creation of CPU lost.\n");
    } else {
        fprintf(logfile, "CPU [%p] created in %s in func %s line %d.\n", 
                cpu, cpu->logs->file_of_creation, 
                cpu->logs->func_of_creation, cpu->logs->line_of_creation);
    }

    fprintf(logfile, "CPU info:\n");

    fprintf(logfile, "\tCPU version: %d\n", PROC_VER);
    fprintf(logfile, "\tAss version: %d\n", cpu->ass_version);
    fprintf(logfile, "\tCode info:\n");
    fprintf(logfile, "\t\tCode length in symbols: %zu\n", cpu->code_len);
    fprintf(logfile, "\t\tAmount of commands:     %d\n",  cpu->amount_of_cmds);

    if (cpu->code == nullptr) {
        fprintf(logfile, "\t\tError: pointer to array with code is null, can't print it\n");
    } else {
        fprintf(logfile, "\t\tCode:\n\t\t{");
        for (size_t ip = 0; cpu->code[ip] != '\0' && ip < cpu->code_len; ++ip) {
            fprintf(logfile, "%c", cpu->code[ip]);
        }
        fprintf(logfile, "}\n");
        fprintf(logfile, "\t\t~");
        for (size_t ip = 0; cpu->code[ip] != '\0' && ip < cpu->code_len && ip < cpu->ip; ++ip) {
            fprintf(logfile, "~");
        }
        fprintf(logfile, "^ ip = %zu\n", cpu->ip);
    }

    if (cpu->cpu_stack == nullptr) {
        fprintf(logfile, "\t\tError: null pointer to cpu stack, can't print stack info\n");
    } else {
        fprintf(logfile, "\t\tStack info:\n\n");
        DumpLogs(cpu->cpu_stack, logfile);
    }

    fflush(logfile);
}
