#ifndef ASSMBLR
#define ASSMBLR

#include "..\Stack\stack.h"
#include "..\Stack\stack_logs.h"
#include "..\Stack\stack_verification.h"

typedef struct {
    char* cmd_ptr;
    char* val_ptr;
    int   cmd_len;
} Command;

void assemble(Command* commands, int amount_of_strings);
void do_command(FILE *output1, Command* commands, int ncommand);
void place_pointers(Command commands[], char *text, size_t amount_of_symbols, 
                                                       int amount_of_strings);

#endif
