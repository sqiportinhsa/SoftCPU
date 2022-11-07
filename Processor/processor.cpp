#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <unistd.h>

#include "processor.hpp"
#include "../Common/file_reading.hpp"
#include "../Stack/stack.h"
#include "../Stack/stack_logs.h"
#include "../Stack/stack_verification.h"

static const char* get_input_filename(int argc, const char **argv);
static void get_amount_of_cmds(CPU *cpu);
static bool binary_verification(CPU *cpu);

static int pop (CMD cmd, CPU *cpu, char cmd_reg, int cmd_val);
static int push(CMD cmd, CPU *cpu, char cmd_reg, int cmd_val);
static int calc_sqrt(CPU *cpu);

static void init_video     (CPU *cpu);
static void destruct_video (CPU *cpu);
static void render_video   (CPU *cpu);

static int sleep_cmd(CPU *cpu);

const char *default_input = "output.bin";


#define DEF_CMD(name, val, args, ...) \
    case val:                         \
        __VA_ARGS__                   \
        break;                        \

int calculate(CPU *cpu) {
    assert(cpu != nullptr);

    int cpu_err   = NO_CPU_ERR;

    while (cpu->ip < cpu->code_len) {

        CMD cmd = *((CMD*) (cpu->code + cpu->ip));
        cpu->ip += sizeof(char);

        if (cmd.cmd == CMD_HLT) {
            break;
        }

        int  val_for_push  = 0;
        int  first__popped = 0;
        int  second_popped = 0;
        int  cmd_val       = 0;
        char cmd_reg       = 0;

        if (cmd.val) {
            cmd_val  = *((int*)  (cpu->code + cpu->ip));
            cpu->ip += sizeof(int);
        }

        if (cmd.reg) {
            cmd_reg  = *((char*) (cpu->code + cpu->ip));
            cpu->ip += sizeof(char);
        }

        if (cmd.ram) {
            if (cmd.reg) {
                cmd_val += cpu->registers[(int) cmd_reg];
            }
        }

        switch (cmd.cmd) {
            #include "../Common/commands.hpp"
            default:
                fprintf(stderr, "Error: undefined command\n");
                cpu_err |= UNDEF_CMD;
                break;
        }

        if (cpu->stk_err != 0) {
            cpu_err |= STACK_ERR;
        }
    }

    return cpu_err;
}

#undef DEF_CMD

static int pop(CMD cmd, CPU *cpu, char cmd_reg, int cmd_val) {
    assert(cpu != nullptr);

    int popped = 0;
    int errors = 0;

    popped |= StackPop(cpu->cpu_stack, &errors);

    if (errors) {
        return STACK_ERR;
    }

    if (cmd.ram) {

        cpu->ram[cmd_val] = popped;

    } else if (cmd.reg) {

        cpu->registers[(int) cmd_reg] = popped;

    } else {

        return INCORR_POP;
    }
    
    return NO_CPU_ERR;
}

static int push(CMD cmd, CPU *cpu, char cmd_reg, int cmd_val) {
    assert(cpu != nullptr);

    int val_for_push = cmd_val;

    if (cmd.reg) {
        val_for_push = cpu->registers[(int) cmd_reg];
    }

    if (cmd.ram) {
        val_for_push = cpu->ram[cmd_val];
    }

    cpu->stk_err |= StackPush(cpu->cpu_stack, val_for_push);

    if (cpu->stk_err) {
        return STACK_ERR;
    }

    return NO_CPU_ERR;
}

static int calc_sqrt(CPU *cpu) {
    assert(cpu != nullptr);
    
    int first__popped  = StackPop(cpu->cpu_stack, &cpu->stk_err);

    if (cpu->stk_err) {
        return STACK_ERR;
    }

    int val_for_push  = (int) sqrt((double) first__popped);

    cpu->stk_err |= StackPush(cpu->cpu_stack, val_for_push);

    if (cpu->stk_err) {
        return STACK_ERR;
    }

    return NO_CPU_ERR;
}

static int sleep_cmd(CPU *cpu) {
    int errors = NO_CPU_ERR;

    int delay = StackPop(cpu->cpu_stack, &errors);

    if (errors) {
        return STACK_ERR;
    }

    usleep(delay);

    return NO_CPU_ERR;
}

int prepare_cpu(CPU *cpu, int argc, const char **argv) {
    int errors = NO_CPU_ERR;

    const char *input_filename = get_input_filename(argc, argv);

    size_t code_len = count_elements_in_file(input_filename);

    errors |= CPU_constructor(cpu, code_len);

    if (errors) {
        return errors;
    }

    cpu->code_len = read_file(cpu->code, cpu->code_len, input_filename);

    if (!binary_verification(cpu)) {
        return INCORR_BINARY;
    }

    get_amount_of_cmds(cpu);

    return NO_CPU_ERR;
    
}

static const char* get_input_filename(int argc, const char **argv) {

    assert(argv != nullptr);

    CLArgs clargs = parse_cmd_line(argc, argv);

    if (clargs.input == nullptr) {
        clargs.input = default_input;
    }

    return clargs.input; 
}

static void get_amount_of_cmds(CPU *cpu) {
    assert(cpu       != nullptr);
    assert(cpu->code != nullptr);

    cpu->amount_of_cmds = *((int*) (cpu->code + cpu->ip));
    cpu->ip += sizeof(int);
}

static bool binary_verification(CPU *cpu) {
    cpu->verification_const = *((int*) (cpu->code + cpu->ip));
    cpu->ip += sizeof(int);
    
    if (cpu->verification_const != Verification_const) {
        fprintf(stderr, "Error: incorrect binary file, verification constant doesn't match.\n");
        return false;
    }

    cpu->ass_version = *((int*) (cpu->code + cpu->ip));
    cpu->ip += sizeof(int);

    if (cpu->ass_version > Proc_version) {
        fprintf(stderr, "Error: processor version is less than assembler version\n");
        return false;
    }

    return true;
}

#define allocate_memory(pointer, type, amount)                                  \
        pointer = (type*) calloc(amount, sizeof(type));                         \
        if (pointer == nullptr) {                                               \
            fprintf(stderr, "error: not enought memory in CPU constructor\n");  \
            CPU_destructor(cpu);                                                \
            return MEMORY_EXC;                                                  \
        }

int real_CPU_constructor(CPU *cpu, size_t code_len, int line, const char* func, const char* file) {
    if (cpu == nullptr) {
        return NULLPTR_ERR;
    }

    allocate_memory(cpu->cpu_stack, Stack, 1);

    StackCtr(cpu->cpu_stack, 0);

    allocate_memory(cpu->adr_stack, Stack, 1);

    StackCtr(cpu->adr_stack, 0);

    allocate_memory(cpu->code, char, code_len);

    allocate_memory(cpu->logs, CPU_logs, 1);

    allocate_memory(cpu->ram,       int, RAM_size);

    allocate_memory(cpu->registers, int, Reg_amount);


    cpu->logs->line_of_creation = line;
    cpu->logs->file_of_creation = file;
    cpu->logs->func_of_creation = func;

    cpu->code_len = code_len;

    #ifdef VIDEO
    init_video(cpu);
    #endif

    return NO_CPU_ERR;
}

int CPU_destructor(CPU *cpu) {
    if (cpu == nullptr) {
        return NULLPTR_ERR;
    }

    StackDestr(cpu->cpu_stack);
    StackDestr(cpu->adr_stack);

    free(cpu->code);
    free(cpu->cpu_stack);
    free(cpu->logs);
    free(cpu->adr_stack);
    free(cpu->ram);
    free(cpu->registers);

    cpu->code       = nullptr;
    cpu->cpu_stack  = nullptr;
    cpu->logs       = nullptr;
    cpu->adr_stack  = nullptr;
    cpu->ram        = nullptr;
    cpu->registers  = nullptr;

    cpu->amount_of_cmds = 0;
    cpu->code_len       = 0;
    cpu->ass_version    = 0;
    cpu->ip             = 0;

    #ifdef VIDEO
    destruct_video(cpu);
    #endif

    return NO_CPU_ERR;
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

    fprintf(logfile, "\tCPU version: %d\n", Proc_version);
    fprintf(logfile, "\tAss version: %d\n", cpu->ass_version);
    fprintf(logfile, "\tCode info:\n");
    fprintf(logfile, "\t\tCode length in symbols: %zu\n", cpu->code_len);
    fprintf(logfile, "\t\tAmount of commands:     %d\n",  cpu->amount_of_cmds);

    if (cpu->code == nullptr) {
        fprintf(logfile, "\t\tError: pointer to array with code is null, can't print it\n");
    } else {
        fprintf(logfile, "\t\tCode:\n\t\t{");
        for (size_t ip = 0; ip < cpu->code_len; ++ip) {
            fprintf(logfile, "%c", cpu->code[ip]);
        }
        fprintf(logfile, "}\n");
        fprintf(logfile, "\t\t~");
        for (size_t ip = 0; ip < cpu->code_len && ip < cpu->ip; ++ip) {
            fprintf(logfile, "~");
        }
        fprintf(logfile, "^ ip = %zu\n", cpu->ip);
    }

    if (cpu->cpu_stack == nullptr) {
        fprintf(logfile, "\t\tError: null pointer to cpu stack, can't print stack info\n");
    } else {
        fprintf(logfile, "\t\tStack info:\n\n");
        RealDumpLogs(cpu->cpu_stack, logfile, file, func, line, StackVerificator(cpu->cpu_stack));
    }

    fflush(logfile);
}

static void init_video(CPU *cpu) {
    SDL_Init (SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer (Screen_width, Screen_height, 0, &cpu->window, &cpu->renderer);
    SDL_SetRenderDrawColor (cpu->renderer, 0, 0, 0, 255);
    SDL_RenderClear (cpu->renderer);
    SDL_RenderPresent (cpu->renderer);
}

static void destruct_video(CPU *cpu) {
    SDL_DestroyRenderer (cpu->renderer);
    SDL_DestroyWindow   (cpu->window);
    SDL_Quit ();

    cpu->renderer = nullptr;
    cpu->window   = nullptr;
}

static void render_video(CPU *cpu) {
    assert(cpu != nullptr);

    Cell_color cell = {};

    SDL_Rect rect = {};

    rect.h = Sqr_size;
    rect.w = Sqr_size;

    SDL_SetRenderDrawColor (cpu->renderer, 0, 0, 0, 255);
    SDL_RenderClear        (cpu->renderer);

    for (int i = 0; i < Screen_height; ++i) {    //vertical   coordinate
        for (int j = 0; j < Screen_width; ++j) { //horisontal coordinate

            cell.arg = cpu->ram[Screen_width*i + j];

            rect.x = j * Sqr_size;
            rect.y = i * Sqr_size;

            SDL_SetRenderDrawColor(cpu->renderer, cell.color.r,
                                                  cell.color.g,
                                                  cell.color.b, 255);

            SDL_RenderFillRect(cpu->renderer, &rect);
        }
    }

    SDL_RenderPresent(cpu->renderer);
}
