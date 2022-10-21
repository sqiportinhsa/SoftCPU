#include <string.h>
#include <ctype.h>

#include "ass.h"
#include "..\Common\common.h"
#include "..\Common\file_reading.h"

int assemble(Command* commands, int amount_of_strings) {
    if (commands == nullptr) {
        return UNEXP_NULLPTR;
    }

    int ncommand = 0;
    int errors   = 0;

    FILE *output = fopen("assembled.bin", "wb");

    if (output == nullptr) {
        fprintf(stderr, "error: can't open file for output");
        errors |= CANT_OPEN_FILE;
        return errors;
    }

    fwrite(&Verification_const, sizeof(int), 1, output);
    fwrite(&Ass_version,        sizeof(int), 1, output);
    fwrite(&amount_of_strings,  sizeof(int), 1, output);

    while (ncommand < amount_of_strings) {
        ASS_DEBUG("start %d\n", ncommand);
        errors |= do_command(output, &commands[ncommand]);
        ASS_DEBUG("done  %d\n", ncommand);
        ++ncommand;
    }

    ASS_DEBUG("all done");

    fclose(output);

    return errors;
}

int do_command(FILE *output, Command *command) {
    if (output == nullptr || command == nullptr) {
        return UNEXP_NULLPTR;
    }

    int len = command->cmd_len;
    int errors = NO_ASS_ERR;

    char cmd[Max_cmd_len] = {};
    memcpy(cmd, command->cmd_ptr, len);
    cmd[len] = '\0';

    command->cmd = 0;
    command->val = 0;
    command->reg = 0;

    if (stricmp("PUSH", cmd) == 0) {
        command->cmd = PUSH_CMD;
        
        if (command->val_ptr == nullptr) {
            fprintf(stderr, "incorrect push: argument expected\n");
            return MISSING_ARG;
        }

        int shift = 0;

        if (*command->val_ptr <= '9' && *command->val_ptr > '0') {
            errors |= get_args_with_first_val(command, &shift);
        } else if (*command->val_ptr == 'r') {
            errors |= get_args_with_first_reg(command, &shift);

            if (command->cmd & VAL) {
                fprintf(stderr, "incorrect pop: unexpected value\n");
                errors |= INCORRECT_VAL;
                return errors;
            }

        } else if (*command->val_ptr == '[') {
            ++shift;

            errors |= get_args_with_first_val(command, &shift);
            errors |= get_args_with_first_reg(command, &shift);

            if (shift == 0) {
                fprintf(stderr, "incorrect push: argument in [] expected\n");
                errors |= INCR_BR_USAGE;
                return errors;
            }
            
            if (*(command->val_ptr + shift) != ']') {
                fprintf(stderr, "incorrect push argument: ] expected\n");
                errors |= INCR_BR_USAGE;
                return errors;
            }

            command->cmd |= RAM;
        } else {
            fprintf(stderr, "incorrect push argument\n");
            errors |= INCORRECT_ARG;
            return errors;
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
    } else if (stricmp("POP", cmd) == 0) {
        command->cmd = POP_CMD;

        if (command->val_ptr == nullptr) {
            fprintf(stderr, "incorrect pop: argument expected\n");
            return MISSING_ARG;
        }

        int shift = 0;

        if (*command->val_ptr != 'r') {
            fprintf(stderr, "incorrect pop argument: register or ram expected\n");
            return INCORRECT_ARG;
        }

        if (*(command->val_ptr + 3) == ' ' || *(command->val_ptr + 3) == '\n') {
            errors |= get_args_with_first_reg(command, &shift);

            if (errors & INCORRECT_REG) {
                fprintf(stderr, "incorrect pop: invalid register\n");
                return errors;
            }

            if (command->cmd & VAL) {
                fprintf(stderr, "incorrect pop: unexpected value\n");
                errors |= INCORRECT_VAL;
                return errors;
            }

            command->cmd |= REG;

        } else if (*(command->val_ptr + 1) == 'a' && 
                   *(command->val_ptr + 2) == 'm' && 
                   *(command->val_ptr + 3) == '[') {

            shift += 4;
            
            errors |= get_args_with_first_val(command, &shift);

            if (command->cmd & REG) {
                fprintf(stderr, "incorrect pop: unexpected register\n");
                errors |= INCORRECT_VAL;
                return errors;
            }

            if (shift == 4) {
                fprintf(stderr, "incorrect pop: argument in [] expected\n");
                errors |= INCR_BR_USAGE;
                return errors;
            }
            
            if (*(command->val_ptr + shift) != ']') {
                fprintf(stderr, "incorrect pop argument: ] expected\n");
                errors |= INCR_BR_USAGE;
                return errors;
            }

            command->cmd |= RAM;
        } else {
            fprintf(stderr, "incorrect pop argument\n");
            errors |= INCORRECT_ARG;
            return errors;
        }
    } else {
        fprintf(stderr, "invalid command\n");
        return INCORRECT_CMD;
    }

    fwrite(&command->cmd, sizeof(char), 1, output);

    if (command->cmd & VAL) {
        fwrite(&command->val, sizeof(int), 1, output);
    }

    if (command->cmd & REG) {
        fwrite(&command->reg, sizeof(char), 1, output);
    }

    fflush(output);
    fflush(stderr);

    return errors;
}

int place_pointers(Command commands[], char *text, size_t amount_of_symbols, 
                                                       int amount_of_strings) {
    if (text == nullptr || commands == nullptr) {
        return UNEXP_NULLPTR;
    }

    commands[0].cmd_ptr = &(text[0]);
    int nstring = 0;
    size_t nsym = 0;

    while (nstring < amount_of_strings && nsym < amount_of_symbols) {
        int cmd_len = 0;
        ASS_DEBUG("start\n");

        nsym += skip_spaces(text + nsym);

        commands[nstring].cmd_ptr = &(text[nsym]);
        ASS_DEBUG("number of command %d first sym: %lld. First symbol: %c\n", nstring, nsym, text[nsym]);

        for (; isalpha(text[nsym]); ++nsym, ++cmd_len);

        commands[nstring].cmd_len = cmd_len;

        for (; text[nsym] == ' '; ++nsym);

        if (text[nsym] == '\n' || text[nsym] == '\0') {
            commands[nstring].val_ptr = nullptr;
            ++nsym;
            ++nstring;
            ASS_DEBUG("there is no value for this command.\n");
            continue;
        }

        commands[nstring].val_ptr = &(text[nsym]);
        ASS_DEBUG("number of value's position: %lld\n", nsym);

        for (; text[nsym] != '\n'; ++nsym);

        ++nsym;
        ++nstring;
    }

    return NO_ASS_ERR;
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

int get_args_with_first_reg(Command *command, int *shift) {
    int errors = NO_ASS_ERR;

    if (*(command->val_ptr + *shift) == 'r') {
        command->reg = get_reg_num(command->val_ptr + *shift);
        if (command->reg == -1) {
            fprintf(stderr, "incorrect push arguments: invalid register\n");
            errors |= INCORRECT_REG;
            return errors;
        }

        command->cmd |= REG;

        *shift += 3;
        *shift += skip_spaces(command->val_ptr + *shift);

        if (*(command->val_ptr + *shift) != '\n' && 
            *(command->val_ptr + *shift) != ';'  &&
            *(command->val_ptr + *shift) != ']') {

            if (*(command->val_ptr + *shift) != '+') {
                fprintf(stderr, "incorrect push arguments: invalid symbol after register\n");
                errors |= UNIDENTIF_SYM;
                return errors;
            }

            ++(*shift);
            *shift += skip_spaces(command->val_ptr + *shift);

            if (*(command->val_ptr + *shift) > '0' && *(command->val_ptr + *shift) <= '9') {
                *shift += get_val(command->val_ptr + *shift, &command->val);
                command->cmd |= VAL;

            } else {
                fprintf(stderr, "incorrect push arguments: expexted value addition\n");
                errors |= INCORRECT_VAL;
                return errors;
            }
        }
    }

    return errors;
}

int get_args_with_first_val(Command *command, int *shift) {
    int errors = NO_ASS_ERR;

    if (*(command->val_ptr + *shift) <= '9' && *(command->val_ptr + *shift) > '0') {
        *shift += get_val(command->val_ptr + *shift, &command->val);
        *shift += skip_spaces(command->val_ptr + *shift + *shift);

        command->cmd |= VAL;

        if (*(command->val_ptr + *shift) != '\n' && 
            *(command->val_ptr + *shift) != ';'  &&
            *(command->val_ptr + *shift) != ']') {

            if (*(command->val_ptr + *shift) != '+') {
                fprintf(stderr, "incorrect push arguments: unexpected sybol after value\n");
                errors |= UNIDENTIF_SYM;
                return errors;
            }

            ++(*shift);
            *shift += skip_spaces(command->val_ptr + *shift);

            command->reg = get_reg_num(command->val_ptr + *shift);
            if (command->reg == -1) {
                fprintf(stderr, "incorrect push arguments: invalid register addition\n");
                errors |= INCORRECT_REG;
                return errors;
            }

            command->cmd |= REG;
            *shift += 3;
        }
    }

    return errors;
}
