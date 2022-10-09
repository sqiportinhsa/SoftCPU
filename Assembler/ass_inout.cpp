#include <stdio.h>
#include <ctype.h>
#include <sys\stat.h>

#include "ass_inout.h"

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
        if (text[i] == '\n') {
            ++amount_of_strings;
        }
    }
    return amount_of_strings;
}

void place_pointers(Command commands[], char *text, size_t amount_of_symbols, 
                                                       int amount_of_strings) {
    commands[0].cmd_ptr = &(text[0]);
    int nstring = 0;
    size_t nsym = 0;

    while (nstring < amount_of_strings && nsym < amount_of_symbols) {
        int cmd_len = 0;

        for (; text[nsym] == ' '; ++nsym);

        commands[nstring].cmd_ptr = &(text[nsym]);
        //printf("number of command %d first sym: %lld. First symbol: %c\n", nstring, nsym, text[nsym]);

        for (; isalpha(text[nsym]); ++nsym, ++cmd_len);

        commands[nstring].cmd_len = cmd_len;

        for (; text[nsym] == ' '; ++nsym);

        if (text[nsym] == '\n') {
            commands[nstring].val_ptr = nullptr;
            ++nsym;
            ++nstring;
            //printf("there is no value for this command.\n");
            continue;
        }

        commands[nstring].val_ptr = &(text[nsym]);
        //printf("number of value's position: %lld\n", nsym);

        for (; text[nsym] != '\n'; ++nsym);

        ++nsym;
        ++nstring;
    }
}

size_t count_elements_in_file(char file_name[]) {
    struct stat a = {};
    stat(file_name, &a);
    return a.st_size;
}
