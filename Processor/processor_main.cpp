#include <stdio.h>
#include <stdlib.h>

#include "processor.h"
#include "..\file_reading.h"

int main() {
    size_t amount_of_elements = count_elements_in_file("assembled.txt");

    char* text = (char*) calloc(amount_of_elements, sizeof(char));
    if (text == nullptr) {
        printf("memory limit exceed");
        return -1;
    }

    amount_of_elements = read_file(text, amount_of_elements, "assembled.txt");

    /*for (size_t i = 0; i < amount_of_elements; ++i) {
        printf("<%c>", text[i]);
    }*/
    
    int amount_of_commands = count_strings(text, amount_of_elements);

    //printf("%d", amount_of_commands);

    Command *commands = (Command*) calloc(amount_of_commands, sizeof(Command));
    if (commands == nullptr) {
        printf("memory limit exceed");
        return -1;
    }

    get_commands(text, commands, amount_of_commands, amount_of_elements);

    free(text);

    calculate(commands, amount_of_commands);

    free(commands);

    return 0;
}