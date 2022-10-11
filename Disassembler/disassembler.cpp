#include <stdio.h>

#include "disassembler.h"
#include "..\Common\file_reading.h"

void disassemble(char *text, int amount_of_commands, size_t amount_of_symbols) {
    FILE *output = fopen("disassembled.txt", "w");

    int n_command = 0;
    size_t n_elem = 0;

    

    while (n_command < amount_of_commands && n_elem < amount_of_symbols) {
        int cmd = 0;
        n_elem += get_val(&(text[n_elem]), &cmd);
        ++n_elem;

        int cmd_val = 0;

        switch (cmd) {
            case PUSH_CMD:
                n_elem += get_val(&(text[n_elem]), &cmd_val);
                ++n_elem;
                fprintf(output, "PUSH %d\n", cmd_val);
                break;
            case ADD_CMD:
                fprintf(output, "ADD\n");
                break;
            case SUB_CMD:
                fprintf(output, "SUB\n");
                break;
            case MUL_CMD:
                fprintf(output, "MUL\n");
                break;
            case DIV_CMD:
                fprintf(output, "DIV\n");
                break;
            case OUT_CMD:
                fprintf(output, "OUT\n");
                break;
            case HLT_CMD:
                fprintf(output, "HLT");
                break;
            default:
                break;
        }

        if (cmd == HLT_CMD) {
            break;
        }

        ++n_command;
    }
}
