#include <stdio.h>
#include <stdlib.h>

#include "ass.h"
#include "..\Stack\stack.h"
#include "..\Stack\stack_logs.h"
#include "..\Stack\stack_verification.h"
#include "..\file_reading.h"

int main() {
    size_t amount_of_elements = count_elements_in_file("input.txt");

    char* text = (char*) calloc(amount_of_elements, sizeof(char));

    if (text == nullptr) {
        printf("memory limit exceed");
        return -1;
    }

    amount_of_elements = read_file(text, amount_of_elements, "input.txt");

    /*for (size_t i = 0; i < amount_of_elements; ++i) {
        printf("<%c>: <%d>\n", text[i], text[i]);
    }*/
    
    int amount_of_strings = count_strings(text, amount_of_elements);

    Command *commands = (Command*) calloc(amount_of_strings, sizeof(Command));

    place_pointers(commands, text, amount_of_elements, amount_of_strings);

    /*for (int i = 0; i < amount_of_strings; ++i) {
        printf("%s: ", commands[i].cmd_ptr);
        if (commands[i].val_ptr != nullptr) {
            printf("%c\n", *commands[i].val_ptr);
        }
    }*/

    assemble(commands, amount_of_strings);
    return 0;
}