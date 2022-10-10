#include <string.h>
#include <ctype.h>

#include "ass.h"
#include "..\common.h"
#include "..\file_reading.h"

void assemble(Command* commands, int amount_of_strings) {
    int ncommand = 0;

    FILE *output = fopen("assembled.txt", "w");

    while (ncommand < amount_of_strings) {
        //printf("start %d\n", ncommand);
        do_command(output, commands, ncommand);
        //printf("done  %d\n", ncommand);
        ++ncommand;
    }
    //printf("all done");
    fclose(output);
}

void do_command(FILE *output, Command* commands, int ncommand) {

    int len = commands[ncommand].cmd_len;

    char cmd[5] = {};
    memcpy(cmd, commands[ncommand].cmd_ptr, len);
    cmd[len] = '\0';

    if (stricmp("PUSH", cmd) == 0) {
        if (commands[ncommand].val_ptr == nullptr) {
            printf("incorrect push: value expected\n");
            return;
        }
        fprintf(output, "%d %d\n", PUSH_CMD, get_val(commands[ncommand].val_ptr));

    } else if (stricmp("ADD", cmd) == 0) {
        fprintf(output, "%d\n", ADD_CMD);

    } else if (stricmp("SUB", cmd) == 0) {
        fprintf(output, "%d\n", SUB_CMD);

    } else if (stricmp("MUL", cmd) == 0) {
        fprintf(output, "%d\n", MUL_CMD);

    } else if (stricmp("DIV", cmd) == 0) {
        fprintf(output, "%d\n", DIV_CMD);

    } else if (stricmp("OUT", cmd) == 0) {
        fprintf(output, "%d\n", OUT_CMD);

    } else if (stricmp("HLT", cmd) == 0) {
        fprintf(output, "%d",   HLT_CMD);

    } else {
        printf("invalid command\n");
    }
}

void place_pointers(Command commands[], char *text, size_t amount_of_symbols, 
                                                       int amount_of_strings) {
    commands[0].cmd_ptr = &(text[0]);
    int nstring = 0;
    size_t nsym = 0;

    while (nstring < amount_of_strings && nsym < amount_of_symbols) {
        int cmd_len = 0;
        //printf("start\n");

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
