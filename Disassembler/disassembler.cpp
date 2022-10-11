#include <stdio.h>

#include "disassembler.h"

void disassemble(Command *commands, int amount_of_commands) {
    int n_command = 0;

    FILE *output = fopen("disassembled.txt", "w");

    while (n_command < amount_of_commands) {
        switch (commands[n_command].cmd) {
            case PUSH_CMD:
                fprintf(output, "PUSH %d\n", commands[n_command].val);
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

        if (commands[n_command].cmd == HLT_CMD) {
            break;
        }

        ++n_command;
    }
}
