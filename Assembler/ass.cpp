#include <string.h>

#include "..\common.h"
#include "ass.h"

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
        fprintf(output, "%d %d\n", PUSH, calculate_val(commands[ncommand].val_ptr));

    } else if (stricmp("ADD", cmd) == 0) {
        fprintf(output, "%d\n", ADD);

    } else if (stricmp("SUB", cmd) == 0) {
        fprintf(output, "%d\n", SUB);

    } else if (stricmp("MUL", cmd) == 0) {
        fprintf(output, "%d\n", MUL);

    } else if (stricmp("DIV", cmd) == 0) {
        fprintf(output, "%d\n", DIV);

    } else if (stricmp("OUT", cmd) == 0) {
        fprintf(output, "%d\n", OUT);

    } else if (stricmp("HLT", cmd) == 0) {
        fprintf(output, "%d",   HLT);

    } else {
        printf("invalid command\n");
    }
}

int calculate_val(char *val_ptr) {
    int val = 0;

    for (int i = 0; *(val_ptr + i) >= '0' && *(val_ptr + i) <= '9'; ++i) {
        val = val*10 + (*(val_ptr + i) - '0');
    }

    return val;
}
