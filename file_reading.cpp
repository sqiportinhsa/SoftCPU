#include <stdio.h>
#include <sys\stat.h>

#include "file_reading.h"

size_t count_elements_in_file(const char file_name[]) {
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

int get_val(char *val_ptr, size_t *val_ptr_ptr) {
    int val     = 0;
    int val_len = 0;

    for (; *(val_ptr + val_len) >= '0' && *(val_ptr + val_len) <= '9'; ++val_len) {
        val = val*10 + (*(val_ptr + val_len) - '0');
    }

    if (val_ptr_ptr != nullptr) {
        *val_ptr_ptr += val_len;
    }

    return val;
}