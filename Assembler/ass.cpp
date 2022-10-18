#include <string.h>
#include <ctype.h>

#include "ass.h"
#include "..\Common\common.h"
#include "..\Common\file_reading.h"

void assemble(Command* commands, int amount_of_strings) {
    if (commands == nullptr) {
        return;
    }

    int ncommand = 0;

    FILE *output = fopen("assembled.bin", "wb");

    if (output == nullptr) {
        printf("error: can't open file for output");
        return;
    }

    fprintf(output, "%d%d", ASS_VER, amount_of_strings);

    while (ncommand < amount_of_strings) {
        //printf("start %d\n", ncommand);
        do_command(output, &commands[ncommand]);
        //printf("done  %d\n", ncommand);
        ++ncommand;
    }
    //printf("all done");
    fclose(output);
}

void do_command(FILE *output, Command *command) {

    int len = command->cmd_len;

    char cmd[5] = {};
    memcpy(cmd, command->cmd_ptr, len);
    cmd[len] = '\0';

    if (stricmp("PUSH", cmd) == 0) {
        if (command->val_ptr == nullptr) {
            printf("incorrect push: value expected\n");
            return;
        }

        int shift = 0;

        if (*command->val_ptr <= '9' && *command->val_ptr > '0') {
            get_args_with_first_val(command, &shift);
        } else if (*command->val_ptr == 'r') {
            get_args_with_first_reg(command, &shift);
        } else if (*command->val_ptr == '[') {
            get_args_with_first_val(command, &shift);
            get_args_with_first_reg(command, &shift);

            if (shift == 0) {
                printf("incorrect push: argument in [] expected\n");
            }
            
            if (*(command->val_ptr + shift) != ']') {
                printf("incorrect push argument: ] expected\n");
                return;
            }
        } else {
            printf("incorrect push argument\n");
            return;
        }

        fprintf(output, "%c", command->cmd);
        if (command->cmd & VAL) {
            fprintf(output, "%d", command->val);
        }
        if (command->cmd & REG) {
            fprintf(output, "%c", command->reg);
        }

    } else if (stricmp("ADD", cmd) == 0) {
        fprintf(output, "%d", ADD_CMD);

    } else if (stricmp("SUB", cmd) == 0) {
        fprintf(output, "%d", SUB_CMD);

    } else if (stricmp("MUL", cmd) == 0) {
        fprintf(output, "%d", MUL_CMD);

    } else if (stricmp("DIV", cmd) == 0) {
        fprintf(output, "%d", DIV_CMD);

    } else if (stricmp("OUT", cmd) == 0) {
        fprintf(output, "%d", OUT_CMD);

    } else if (stricmp("HLT", cmd) == 0) {
        fprintf(output, "%d",   HLT_CMD);

    } else {
        printf("invalid command\n");
    }
}

void place_pointers(Command commands[], char *text, size_t amount_of_symbols, 
                                                       int amount_of_strings) {
    if (text == nullptr || commands == nullptr) {
        return;
    }

    commands[0].cmd_ptr = &(text[0]);
    int nstring = 0;
    size_t nsym = 0;

    while (nstring < amount_of_strings && nsym < amount_of_symbols) {
        int cmd_len = 0;
        //printf("start\n");

        nsym += skip_spaces(text + nsym);

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

Register get_reg_num(char *ptr) {
    if (*ptr != 'r' || *(ptr + 2) != 'x') {
        return NOT_REG;
    }

    int reg_num = *(ptr + 1) - 'a' + 1;

    if (reg_num > MIN_REG && reg_num < MAX_REG) {
        return (Register) reg_num;
    }

    return NOT_REG;
}

void get_args_with_first_reg(Command *command, int *shift) {
    command->reg = get_reg_num(command->val_ptr);
    if (command->reg == -1) {
        printf("incorrect push arguments: invalid register");
        return;
    }

    command->cmd |= REG;

    *shift += 3;
    *shift += skip_spaces(command->val_ptr + *shift);

    if (*(command->val_ptr + *shift) > '0' && *(command->val_ptr + *shift) <= '9') {
        *shift += get_val(command->val_ptr + *shift, &command->val);
        command->cmd |= REG;

    } else {
        printf("incorrect push arguments: invalid register after value");
        return;
    }
}

void get_args_with_first_val(Command *command, int *shift) {
    if (*command->val_ptr <= '9' && *command->val_ptr > '0') {
        *shift += get_val(command->val_ptr, &command->val);
        *shift += skip_spaces(command->val_ptr + *shift);

        command->cmd |= VAL;

        if (*(command->val_ptr + *shift) != '\n') {
            command->reg = get_reg_num(command->val_ptr + *shift);
            if (command->reg == -1) {
                printf("incorrect push arguments: invalid register after value");
                return;
            }

            command->cmd |= REG;
            *shift += 3;
        }
    }
}
