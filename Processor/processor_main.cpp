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

    if (!binary_verification(&cpu)) {
        return -1;
    }

    cpu.amount_of_cmds = *((int*) (cpu.code + cpu.ip));
    cpu.ip += sizeof(int);

    errors |= calculate(&cpu);

    fclose(logfile);

    return 0;
}