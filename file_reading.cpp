#include <stdio.h>
#include <sys\stat.h>

#include "file_reading.h"

size_t count_elements_in_file(char file_name[]) {
    struct stat a = {};
    stat(file_name, &a);
    return a.st_size;
}

size_t read_file(char text[], size_t amount_of_symbols, const char* file_name) {
    FILE *input = fopen(file_name, "r");

    size_t nread = fread(text, sizeof(char), amount_of_symbols, input);

    fclose(input);

    size_t real_amount_of_symbols = nread + 1;
    text[real_amount_of_symbols - 1] = '\0';

    return real_amount_of_symbols;
}

int count_strings(char text[], size_t amount_of_symbols) {
    int amount_of_strings = 0;
    for (size_t i = 0; i < amount_of_symbols; ++i) {
        if (text[i] == '\n' || text[i] == '\0') {
            ++amount_of_strings;
        }
    }
    return amount_of_strings;
}

int get_val(char *val_ptr) {
    int val = 0;

    for (int i = 0; *(val_ptr + i) >= '0' && *(val_ptr + i) <= '9'; ++i) {
        val = val*10 + (*(val_ptr + i) - '0');
    }

    return val;
}
