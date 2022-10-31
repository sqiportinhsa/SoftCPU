#include <stdio.h>
#include <stdlib.h>

#include "ass.h"
#include "..\Stack\stack.h"
#include "..\Stack\stack_logs.h"
#include "..\Stack\stack_verification.h"
#include "..\Common\file_reading.h"

int main() {
    int errors = NO_ASS_ERR;

    Ass ass = {};

    ass.amount_of_code_symbols = count_elements_in_file("input.txt");

    ass.code = (char*) calloc(ass.amount_of_code_symbols, sizeof(char));

    if (ass.code == nullptr) {
        fprintf(stderr, "memory limit exceed");
        return -1;
    }

    ass.amount_of_code_symbols = read_file(ass.code, ass.amount_of_code_symbols, "input.txt");

    /*for (size_t i = 0; i < ass.amount_of_code_symbols; ++i) {
        printf("<%c>: <%d>\n", ass.code[i], ass.code[i]);
    }*/

    ass.amount_of_code_strings = count_strings(ass.code, ass.amount_of_code_symbols);


    ass.commands = (Command*) calloc(ass.amount_of_code_strings, sizeof(Command));

    ass.markers  = (Marker*)  calloc(ass.amount_of_code_strings, sizeof(Marker));

    ass.amount_of_code_strings = place_pointers(&ass);

    errors |= verify_markers(ass.markers, ass.amount_of_markers);


    /*for (int i = 0; i < amount_of_strings; ++i) {
        printf("%s: ", commands[i].cmd_ptr);
        if (commands[i].val_ptr != nullptr) {
            printf("%c\n", *commands[i].val_ptr);
        }
    }*/

    errors |= assemble(&ass);
    
    return 0;
}