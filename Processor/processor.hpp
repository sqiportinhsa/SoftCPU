#ifndef PROCESSOR
#define PROCESSOR

#include "../Common/common.hpp"
#include "../Stack/stack.h"

const int Proc_version   = 3;
const int Reg_amount     = 5;
const int RAM_size       = 100;

static const char *default_input = "output.bin";

struct CPU_logs{
    const char* file_of_creation;
    const char* func_of_creation;
    int         line_of_creation;
};

struct CPU {
    CPU_logs*   logs                  = nullptr;
    char*       code                  = nullptr;
    Stack*      cpu_stack             = nullptr;
    Stack*      adr_stack             = nullptr;
    size_t      ip                    = 0;
    size_t      code_len              = 0;
    int         amount_of_cmds        = 0;
    int         ass_version           = 0;
    int         verification_const    = 0;
    int         registers[Reg_amount] = {};
    int         ram[RAM_size]         = {};
    int         stk_err               = NO_ERROR;
};

enum CPU_errors {
    NO_CPU_ERR    = 0,
    NULLPTR_ERR   = 1 << 0,
    MEMORY_EXC    = 1 << 1,
    STACK_ERR     = 1 << 2,
    UNDEF_CMD     = 1 << 3,
    INCORR_POP    = 1 << 4,
    INCORR_BINARY = 1 << 5,
};

#define CPU_constructor(cpu, code_len) real_CPU_constructor(cpu, code_len, __LINE__,     \
                                                            __PRETTY_FUNCTION__, __FILE__)

#define dump_cpu(cpu, logfile) real_dump_cpu(cpu, logfile, __FILE__, __PRETTY_FUNCTION__, __LINE__)

#define RETURN_IF(errors)                           \
        if (errors != 0) {                          \
            fprintf(logfile, "errors: %d", errors); \
            fclose (logfile);                       \
            return -1;                              \
        }

int prepare_cpu(CPU *cpu, int argc, const char **argv);

int calculate(CPU *cpu);

int real_CPU_constructor(CPU *cpu, size_t code_len, int line, const char* func, const char* file);

int CPU_destructor(CPU *cpu);

void real_dump_cpu(CPU *cpu, FILE *logfile, const char *file, const char *func, int line);

#endif