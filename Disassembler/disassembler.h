#ifndef DISASSEMBLER
#define DISASSEMBLER

#include "..\Common\common.h"
#include "..\Common\utils.h"

const int Disasm_version = 3;

typedef struct {
    char*     code                  = nullptr;
    size_t    ip                    = 0;
    size_t    code_len              = 0;
    int       amount_of_cmds        = 0;
    int       ass_version           = 0;
    int       verification_const    = 0;
} Disasm;

void disassemble(Disasm *disasm);
void disasm_constructor(Disasm *disasm, size_t code_len);

#endif