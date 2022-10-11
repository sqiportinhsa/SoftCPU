#include <stdio.h>
#include <stdlib.h>

#include "disassembler.h"
#include "..\Common\file_reading.h"
#include "..\Common\common.h"

int main() {
    size_t amount_of_elements = count_elements_in_file("assembled.txt");

    char* text = (char*) calloc(amount_of_elements, sizeof(char));
    if (text == nullptr) {
        printf("memory limit exceed");
        return -1;
    }

    amount_of_elements = read_file(text, amount_of_elements, "assembled.txt");

    int ip = 0;
    int ass_ver = 0;
    int amount_of_commands = 0;

    ip += get_val(&(text[ip]), &ass_ver);
    ++ip;

    //printf("%d ", ip);

    ip += get_val(&(text[ip]), &amount_of_commands);
    ++ip;

    //printf("%d\n", ip);

    //printf("%d %d\n", ass_ver, amount_of_commands);

    /*for(size_t i = 0; i < amount_of_elements; ++i) {
        printf("<%c>", text[i]);
    }*/

    Command *commands = (Command*) calloc(amount_of_commands, sizeof(Command));
    if (commands == nullptr) {
        printf("memory limit exceed");
        return -1;
    }

    get_commands(&(text[ip]), commands, amount_of_elements - ip - 3);

    free(text);

    disassemble(commands, amount_of_commands);

    free(commands);

    return 0;
}