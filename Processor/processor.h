#ifndef PROCESSOR
#define PROCESSOR

#include "..\Common\common.h"
#include "..\Common\utils.h"
#include "..\Stack\stack.h"

#define PROC_VER 1
#define REG_AMOUNT 5
#define RAM_SIZE 100

typedef struct {
    char*  code                  = nullptr;
    Stack* cpu_stack             = nullptr;
    size_t ip                    = 0;
    size_t code_len              = 0;
    int    amount_of_cmds        = 0;
    int    ass_version           = 0;
    int    registers[REG_AMOUNT] = {};
    int    ram[RAM_SIZE]         = {};
} CPU;

void calculate(CPU *cpu);
void CPU_constructor(CPU *cpu, size_t code_len);
void CPU_destructor(CPU *cpu);

#endif