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
        fprintf(stderr, "error: can't open file for output");
        return;
    }

    fwrite(&verification_const, sizeof(int), 1, output);
    fwrite(&ASS_VER,            sizeof(int), 1, output);
    fwrite(&amount_of_strings,  sizeof(int), 1, output);

    while (ncommand < amount_of_strings) {
        //fprintf(stderr,"start %d\n", ncommand);
        do_command(output, &commands[ncommand]);
        //fprintf(stderr,"done  %d\n", ncommand);
        ++ncommand;
    }
    //fprintf(stderr,"all done");
    fclose(output);
}

void do_command(FILE *output, Command *command) {

    int len = command->cmd_len;

    char cmd[5] = {};
    memcpy(cmd, command->cmd_ptr, len);
    cmd[len] = '\0';

    command->cmd = 0;
    command->val = 0;
    command->reg = 0;

    if (stricmp("PUSH", cmd) == 0) {
        if (command->val_ptr == nullptr) {
            fprintf(stderr, "incorrect push: value expected\n");
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
                fprintf(stderr, "incorrect push: argument in [] expected\n");
            }
            
            if (*(command->val_ptr + shift) != ']') {
                fprintf(stderr, "incorrect push argument: ] expected\n");
                return;
            }
        } else {
            fprintf(stderr, "incorrect push argument\n");
            return;
        }

    } else if (stricmp("ADD", cmd) == 0) {
        command->cmd = ADD_CMD;

    } else if (stricmp("SUB", cmd) == 0) {
        command->cmd = SUB_CMD;

    } else if (stricmp("MUL", cmd) == 0) {
        command->cmd = MUL_CMD;

    } else if (stricmp("DIV", cmd) == 0) {
        command->cmd = DIV_CMD;

    } else if (stricmp("OUT", cmd) == 0) {
        command->cmd = OUT_CMD;

    } else if (stricmp("HLT", cmd) == 0) {
        command->cmd = HLT_CMD;

    } else {
        fprintf(stderr, "invalid command\n");
        return;
    }

    fwrite(&command->cmd, sizeof(char), 1, output);

    if (command->cmd & VAL) {
        fwrite(&command->val, sizeof(int), 1, output);
    }

    if (command->cmd & REG) {
        fwrite(&command->reg, sizeof(char), 1, output);
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
        //fprintf(stderr, "start\n");

        nsym += skip_spaces(text + nsym);

        commands[nstring].cmd_ptr = &(text[nsym]);
        //fprintf(stderr, "number of command %d first sym: %lld. First symbol: %c\n", nstring, nsym, text[nsym]);

        for (; isalpha(text[nsym]); ++nsym, ++cmd_len);

        commands[nstring].cmd_len = cmd_len;

        for (; text[nsym] == ' '; ++nsym);

        if (text[nsym] == '\n') {
            commands[nstring].val_ptr = nullptr;
            ++nsym;
            ++nstring;
            //fprintf(stderr, "there is no value for this command.\n");
            continue;
        }

        commands[nstring].val_ptr = &(text[nsym]);
        //fprintf(stderr, "number of value's position: %lld\n", nsym);

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
        fprintf(stderr, "incorrect push arguments: invalid register\n");
        return;
    }

    command->cmd |= REG;

    *shift += 3;
    *shift += skip_spaces(command->val_ptr + *shift);

    if (*(command->val_ptr + *shift) == '+') {
        ++(*shift);
        *shift += skip_spaces(command->val_ptr + *shift);

        if (*(command->val_ptr + *shift) > '0' && *(command->val_ptr + *shift) <= '9') {
            *shift += get_val(command->val_ptr + *shift, &command->val);
            command->cmd |= REG;

        } else {
            fprintf(stderr, "incorrect push arguments: invalid register after value\n");
            return;
        }
    } else if (*(command->val_ptr + *shift) != '\n' && *(command->val_ptr + *shift) != ';') {
        return;
    }
}

void get_args_with_first_val(Command *command, int *shift) {
    if (*command->val_ptr <= '9' && *command->val_ptr > '0') {
        *shift += get_val(command->val_ptr, &command->val);
        *shift += skip_spaces(command->val_ptr + *shift);

        command->cmd |= VAL;

        if (*(command->val_ptr + *shift) != '\n' && *(command->val_ptr + *shift) != ';') {
            fprintf(stderr, "%c", *(command->val_ptr + *shift));
            if (*(command->val_ptr + *shift) != '+') {
                fprintf(stderr, "incorrect push arguments: unexpected sybol after value\n");
                return;
            }

            ++(*shift);
            *shift += skip_spaces(command->val_ptr + *shift);

            command->reg = get_reg_num(command->val_ptr + *shift);
            if (command->reg == -1) {
                fprintf(stderr, "incorrect push arguments: invalid register after value\n");
                return;
            }

            command->cmd |= REG;
            *shift += 3;
        }
    }
}
