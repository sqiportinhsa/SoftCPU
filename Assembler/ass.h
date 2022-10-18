#ifndef ASSMBLR
#define ASSMBLR

#include "..\Stack\stack.h"
#include "..\Stack\stack_logs.h"
#include "..\Stack\stack_verification.h"

#define ASS_VER 2

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

void assemble(Command* commands, int amount_of_strings);
void do_command(FILE *output1, Command *command);
void place_pointers(Command commands[], char *text, size_t amount_of_symbols, 
                                                       int amount_of_strings);
Register get_reg_num(char *ptr);
void get_args_with_first_reg(Command *command, int *shift);
void get_args_with_first_val(Command *command, int *shift);

#endif
