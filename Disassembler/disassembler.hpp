#ifndef DISASSEMBLER
#define DISASSEMBLER

#include "../Common/common.hpp"
#include "../Common/file_reading.hpp"

const int Disasm_version = 3;

typedef struct {
    char*     code                  = nullptr;
    int*      markers               = nullptr;
    size_t    ip                    = 0;
    size_t    code_len              = 0;
    int       amount_of_cmds        = 0;
    int       amount_of_markers     = 0;
    int       ass_version           = 0;
    int       verification_const    = 0;
} Disasm;

void disassemble(Disasm *disasm, const char *output_name);

bool init_disasm(Disasm *disasm, const char *input_name);

CLArgs get_cmd_line_args(int argc, const char **argv);

void free_disasm(Disasm *disasm);

bool get_markers(Disasm *disasm);

#endif