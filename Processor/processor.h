#ifndef PROCESSOR
#define PROCESSOR

#include "..\Common\common.h"
#include "..\Common\utils.h"
#include "..\Stack\stack.h"

const int Proc_version   = 3;
const int Reg_amount     = 5;
const int RAM_size       = 100;

typedef struct {
    const char* file_of_creation;
    const char* func_of_creation;
    int         line_of_creation;
} CPU_logs;

typedef struct {
    CPU_logs* logs                  = nullptr;
    char*     code                  = nullptr;
    Stack*    cpu_stack             = nullptr;
    size_t    ip                    = 0;
    size_t    code_len              = 0;
    int       amount_of_cmds        = 0;
    int       ass_version           = 0;
    int       verification_const    = 0;
    int       registers[Reg_amount] = {};
    int       ram[RAM_size]         = {};
} CPU;

#define CPU_constructor(cpu, code_len) real_CPU_constructor(cpu, code_len, __LINE__,     \
                                                            __PRETTY_FUNCTION__, __FILE__)

#define dump_cpu(cpu, logfile) real_dump_cpu(cpu, logfile, __FILE__, __PRETTY_FUNCTION__, __LINE__)

void calculate(CPU *cpu);
void real_CPU_constructor(CPU *cpu, size_t code_len, int line, const char* func, const char* file);
void CPU_destructor(CPU *cpu);
void real_dump_cpu(CPU *cpu, FILE *logfile, const char *file, const char *func, int line);

#endif