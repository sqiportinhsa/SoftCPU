#include <stdio.h>
#include <sys\stat.h>
#include <cassert>

#include "file_reading.h"

size_t count_elements_in_file(const char file_name[]) {
    struct stat a = {};
    stat(file_name, &a);
    return a.st_size;
}

size_t read_file(char text[], size_t amount_of_symbols, const char* file_name) {
    assert(text != nullptr && "text is nullptr");
    
    FILE *input = fopen(file_name, "r");

    size_t nread = fread(text, sizeof(char), amount_of_symbols, input);

    fclose(input);

    size_t real_amount_of_symbols = nread + 1;
    text[real_amount_of_symbols - 1] = '\0';

    return real_amount_of_symbols;
}

int count_strings(char text[], size_t amount_of_symbols) {
    assert(text != nullptr && "text is nullptr");

    int amount_of_strings = 0;
    for (size_t i = 0; i < amount_of_symbols; ++i) {
        if (text[i] == '\n' || text[i] == '\0') {
            ++amount_of_strings;
        }
    }
    return amount_of_strings;
}

int get_val(char *ptr_to_first_elem, int *ptr_to_val) {
    assert(ptr_to_first_elem != nullptr && "ptr_to_first_elem is nullptr");
    assert(ptr_to_val        != nullptr && "ptr_to_val is nullptr");

    int val_len = 0;

    for (; *(ptr_to_first_elem + val_len) >= '0' && 
           *(ptr_to_first_elem + val_len) <= '9'; ++val_len) {

        *ptr_to_val = *ptr_to_val*10 + (*(ptr_to_first_elem + val_len) - '0');
    }

    return val_len;
}

void skip_spaces(char **pointer) {
    assert(pointer != nullptr && "pointer is nullptr");

    for (; **pointer == ' '; ++(*pointer));
}

size_t skip_spaces(char *pointer) {
    assert(pointer != nullptr && "pointer in nullptr");

    size_t i = 0;
    for (; *(pointer + i) == ' '; ++i);

    return i;
}

void skip_to_newline(char **pointer) {
    assert(pointer != nullptr && "pointer is nullptr");

    for (; **pointer != '\n'; ++(*pointer));
}

size_t skip_to_newline(char *pointer) {
    assert(pointer != nullptr && "pointer is nullptr");

    size_t i = 0;
    for (; *(pointer + 1) != '\n'; ++i);

    return i;
}
