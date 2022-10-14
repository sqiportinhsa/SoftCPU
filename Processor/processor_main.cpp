#include <stdio.h>
#include <stdlib.h>

#include "processor.h"
#include "..\Common\file_reading.h"

int main() {
    CPU cpu = {};

    size_t code_len = count_elements_in_file("assembled.txt");

    CPU_constructor(&cpu, code_len);

    cpu.code_len = read_file(cpu.code, cpu.code_len, "assembled.txt");

    cpu.ip += get_val(&(cpu.code[cpu.ip]), &cpu.ass_version);
    ++cpu.ip;

    //printf("%d ", cpu.ip);

    cpu.ip += get_val(&(cpu.code[cpu.ip]), &cpu.amount_of_cmds);
    ++cpu.ip;

    //printf("%d\n", cpu.ip);

    //printf("%d %d\n", cpu.ass_version, cpu.amount_of_cmds);

    /*for(size_t i = 0; i < cpu.code_len; ++i) {
        printf("<%c>", cpu.code[i]);
    }*/

    calculate(&cpu);

    CPU_destructor(&cpu);

    return 0;
}