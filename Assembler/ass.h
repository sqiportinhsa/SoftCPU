#ifndef ASSMBLR
#define ASSMBLR

#include "..\Stack\stack.h"
#include "..\Stack\stack_logs.h"
#include "..\Stack\stack_verification.h"

const int Ass_version = 2;
const int Verification_const = 0xDEED;
const int Max_cmd_len = 5;

#define DEBUG_ASS

#ifdef DEBUG_ASS
#define ASS_DEBUG(format, ...) fprintf(stderr, format, ##__VA_ARGS__)
#else
#define ASS_DEBUG(format, ...)
#endif

typedef struct {
    char* cmd_ptr;
    char* val_ptr;
    int   cmd_len;
    char  cmd = 0;
    int   val = 0;
    char  reg = 0;
} Command;

typedef enum {
    NOT_REG = -1,
    MIN_REG = 0,
    RAX = 1,
    RBX = 2,
    RCX = 3,
    RDX = 4,
    MAX_REG = 5,
} Register;

typedef enum {
    NO_ASS_ERR = 0,
    INCORR_CMD = 1 << 1,
    INCORR_REG = 1 << 2,
    INCORR_VAL = 1 << 3,
} AssErrors;

void assemble(Command* commands, int amount_of_strings);
void do_command(FILE *output1, Command *command);
void place_pointers(Command commands[], char *text, size_t amount_of_symbols, 
                                                       int amount_of_strings);
Register get_reg_num(char *ptr);
void get_args_with_first_reg(Command *command, int *shift);
void get_args_with_first_val(Command *command, int *shift);

#endif
