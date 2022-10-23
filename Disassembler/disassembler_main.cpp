#include <stdio.h>
#include <stdlib.h>

#include "disassembler.h"
#include "..\Common\file_reading.h"
#include "..\Common\common.h"

int main() {
    size_t code_len = count_elements_in_file("assembled.bin");

    Disasm disasm = {};

    disasm_constructor(&disasm, code_len);

    disasm.code_len = read_file(disasm.code, disasm.code_len, "assembled.bin");

    disasm.verification_const = *((int*) (disasm.code + disasm.ip));
    disasm.ip += sizeof(int);
    
    if (disasm.verification_const != Verification_const) {
        fprintf(stderr, "Error: incorrect binary file, verification constant doesn't match.\n");
        return -1;
    }

    disasm.ass_version = *((int*) (disasm.code + disasm.ip));
    disasm.ip += sizeof(int);

    if (disasm.ass_version > Disasm_version) {
        fprintf(stderr, "Error: processor version is less than assembler version\n");
        return -1;
    }

    disasm.amount_of_cmds = *((int*) (disasm.code + disasm.ip));
    disasm.ip += sizeof(int);

    //printf("%d\n", ip);

    //printf("%d %d\n", ass_ver, amount_of_commands);

    /*for(size_t i = 0; i < amount_of_elements; ++i) {
        printf("<%c>", text[i]);
    }*/

    disassemble(&disasm);

    return 0;
}