#include <stdio.h>
#include <stdlib.h>

#include "processor.hpp"
#include "../Common/file_reading.hpp"
#include "../Stack/stack_logs.h"

int main() {
    FILE *logfile = fopen("logfile.txt", "w");
    SetLogStream(logfile);

    int errors = NO_CPU_ERR;

    CPU cpu = {};

    size_t code_len = count_elements_in_file("assembled.bin");

    errors |= CPU_constructor(&cpu, code_len);

    cpu.code_len = read_file(cpu.code, cpu.code_len, "assembled.bin");

    cpu.verification_const = *((int*) (cpu.code + cpu.ip));
    cpu.ip += sizeof(int);
    
    if (cpu.verification_const != Verification_const) {
        fprintf(stderr, "Error: incorrect binary file, verification constant doesn't match.\n");
        return -1;
    }

    cpu.ass_version = *((int*) (cpu.code + cpu.ip));
    cpu.ip += sizeof(int);

    if (cpu.ass_version > Proc_version) {
        fprintf(stderr, "Error: processor version is less than assembler version\n");
        return -1;
    }

    cpu.amount_of_cmds = *((int*) (cpu.code + cpu.ip));
    cpu.ip += sizeof(int);

    //printf("%d %d\n", cpu.ass_version, cpu.amount_of_cmds);

    /*for(size_t i = 0; i < cpu.code_len; ++i) {
        fprintf(stderr, "<%d>", cpu.code[i]);
    }*/

    errors |= calculate(&cpu);

    fclose(logfile);

    return 0;
}