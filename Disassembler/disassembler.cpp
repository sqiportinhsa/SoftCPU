#include <stdio.h>
#include <stdlib.h>

#include "disassembler.h"
#include "..\Common\file_reading.h"

void disassemble(Disasm *disasm) {
    FILE *output = fopen("disassembled.txt", "w");

    if (output == nullptr) {
        fprintf(stderr, "can't open output file\n");
    }

    int n_command = 0;

    while (n_command < disasm->amount_of_cmds && disasm->ip < disasm->code_len) {
        fprintf(stderr, "%d", n_command);
        char cmd = *((char*) (disasm->code + disasm->ip));
        disasm->ip += sizeof(char);
        fprintf(stderr, "%d", cmd);

        int  cmd_val = 0;
        char cmd_reg = 0;

        if (cmd & VAL) {
            cmd_val = *((int*)  (disasm->code + disasm->ip));
            disasm->ip += sizeof(int);
        }

        if (cmd & REG) {
            cmd_reg = *((char*) (disasm->code + disasm->ip));
            disasm->ip += sizeof(char);
        }

        switch (cmd & CMD) {
            case PUSH_CMD:
                fprintf(output, "PUSH ");
                if (cmd & RAM) {
                    fprintf(output, "[");
                }
                if (cmd & VAL) {
                    fprintf(output, "%d", cmd_val);
                }
                if (cmd & VAL && cmd & REG) {
                    fprintf(output, " + ");
                }
                if (cmd & REG) {
                    fprintf(output, "%d", cmd_reg);
                }
                if (cmd & RAM) {
                    fprintf(output, "]");
                }
                fprintf(output, "\n");
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
            case POP_CMD:
                fprintf(output, "POP ");
                if (cmd & REG) {
                    fprintf(output, "r%cx\n", 'a' + cmd_reg - 1);
                } else if (cmd & RAM) {
                    fprintf(output, "[%d]\n", cmd_val);
                } else {
                    return;
                }
                break;
            default:
                return;
        }

        if (cmd == HLT_CMD) {
            break;
        }

        ++n_command;
        fflush(output);
    }

    fclose(output);
}

void disasm_constructor(Disasm *disasm, size_t code_len) {
    if (disasm == nullptr) {
        return;
    }

    disasm->code = (char*) calloc(code_len, sizeof(char));
    if (disasm->code == nullptr) {
        fprintf(stderr, "Error: not enough memory for code\n");
        free(disasm->code);
        return;
    }

    disasm->code_len = code_len;
}
