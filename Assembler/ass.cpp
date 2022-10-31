#include <string.h>
#include <ctype.h>
#include <cassert>

#include "ass.h"
#include "..\Common\common.h"
#include "..\Common\file_reading.h"


static int do_command(FILE *output, Command *command, Ass *ass);

static Register get_reg_num(char *ptr);

static int parce_command(Ass *ass, Command *command, size_t *index_in_assembled_code);

static int parce_for_pop  (Command *command, int cmd_num);
static int parce_for_push (Command *command, int cmd_num);
static int parce_for_jump (Command *command, int cmd_num, Ass *ass);

static int get_args_with_first_reg(Command *command, int *shift);
static int get_args_with_first_val(Command *command, int *shift);

static size_t skip_comment(char *pointer);


int assemble(Ass *ass) {
    if (ass == nullptr) {
        return UNEXP_NULLPTR;
    }

    if (ass->commands == nullptr || ass->markers == nullptr || ass->code == nullptr) {
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

    fwrite(&Verification_const,           sizeof(int), 1, output);
    fwrite(&Ass_version,                  sizeof(int), 1, output);
    fwrite(&ass->amount_of_code_strings,  sizeof(int), 1, output);

    while (ncommand < ass->amount_of_code_strings) {
        ASS_DEBUG("start %d\n", ncommand);
        errors |= do_command(output, &ass->commands[ncommand], ass);
        ASS_DEBUG("done  %d\n", ncommand);
        ++ncommand;
    }

    ASS_DEBUG("all done");

    fclose(output);

    return errors;
}

#define DEF_CMD(name, cmd_num, args, ...)                                                          \
    if (stricmp(cmd, #name) == 0) {                                                                \
                                                                                                   \
        if (args == NO_ARGS) {                                                                     \
                                                                                                   \
            command->cmd = cmd_num;                                                                \
                                                                                                   \
        } else if (args == POP__ARGS) {                                                            \
                                                                                                   \
            errors |= parce_for_pop(command, cmd_num);                                             \
                                                                                                   \
            if (errors != 0) {return errors;}                                                      \
                                                                                                   \
        } else if (args == PUSH_ARGS) {                                                            \
                                                                                                   \
            errors |= parce_for_push(command, cmd_num);                                            \
                                                                                                   \
            if (errors != 0) {return errors;}                                                      \
                                                                                                   \
        } else if (args == JUMP_ARGS) {                                                            \
                                                                                                   \
            errors |= parce_for_jump(command, cmd_num, ass);                                       \
                                                                                                   \
            if (errors != 0) {return errors;}                                                      \
        }                                                                                          \
                                                                                                   \
    } else                                                                                       


static int do_command(FILE *output, Command *command, Ass *ass) {

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

    #include "../Common/commands.h"
    {
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

    if (command->jump_to != 0) {

        fwrite(&command->jump_to, sizeof(size_t), 1, output);
    }

    fflush(output);
    fflush(stderr);

    return errors;
}

static int parce_command(Ass *ass, Command *command, size_t *index_in_assembled_code) {
    if (ass == nullptr || index_in_assembled_code == nullptr) {
        return UNEXP_NULLPTR;
    }

    int errors = NO_ASS_ERR;

    int len = command->cmd_len;

    char cmd[Max_cmd_len] = {};

    memcpy(cmd, command->cmd_ptr, len);

    cmd[len] = '\0';

    command->cmd = 0;
    command->val = 0;
    command->reg = 0;

    #include "../Common/commands.h"
    {
        fprintf(stderr, "invalid command\n");
    
        return INCORRECT_CMD;
    }

    int cmd_size = sizeof(char);

    if (command->cmd & VAL) {

        cmd_size += sizeof(int);
    }

    if (command->cmd & REG) {

        cmd_size += sizeof(char);
    }

    *index_in_assembled_code += cmd_size;
    
    return errors;
}

#undef DEF_CMD

static int parce_for_pop(Command *command, int cmd_num) {
    assert (command != nullptr);

    int errors = 0;

    command->cmd = (char) cmd_num;                                                                    
                                                                                           
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
    }

    return errors;
}
      
static int parce_for_push(Command *command, int cmd_num) {

    assert(command != nullptr);
    
    int errors = 0;

    command->cmd = (char) cmd_num;
                                                                                                   
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

    return errors;
}

static int parce_for_jump(Command *command, int cmd_num, Ass *ass) {
    assert (command != nullptr);
    assert (ass     != nullptr);

    command->cmd = (char) cmd_num;

    for (int nmarker = 0; nmarker < ass->amount_of_markers; ++nmarker) {

        if (strnicmp(ass->markers[nmarker].ptr, command->val_ptr, ass->markers[nmarker].len) == 0) {

            command->jump_to = ass->markers[nmarker].index_in_assembled;
            
            break;
        }
    }

    if (command->jump_to == 0) {
        return JMP_TO_NONEXS_MARK;
    }

    return NO_ASS_ERR;
}

int place_pointers(Ass *ass) {
    if (ass->code == nullptr || ass->commands == nullptr) {
        return UNEXP_NULLPTR;
    }

    ass->commands[0].cmd_ptr = &(ass->code[0]);

    int ncommand = 0;
    int  nmarker = 0;
    size_t  nsym = 0;

    size_t position_in_assembled_code = 3 * sizeof(int);

    while (ncommand < ass->amount_of_code_strings && 
            nmarker < ass->amount_of_code_strings &&
               nsym < ass->amount_of_code_symbols) {

        int cmd_len = 0;

        ASS_DEBUG("start\n");

        nsym += skip_spaces(ass->code + nsym);

        nsym += skip_comment(ass->code + nsym);

        ASS_DEBUG("number of command %d first sym: %lld. First symbol: %c\n", ncommand, nsym, ass->code[nsym]);

        if (ass->code[nsym] == ':') { //string is a marker
            ++nsym;
            
            ass->markers[nmarker].ptr     = &(ass->code[nsym]);
            ass->markers[nmarker].nstring = ncommand + 1;

            ass->markers[nmarker].index_in_assembled = position_in_assembled_code;

            ass->markers[nmarker].len += skip_to_newline(ass->code + nsym);

            nsym += ass->markers[nmarker].len;

            ++nsym;
            ++nmarker;

            continue;
        }

        ass->commands[ncommand].cmd_ptr = &(ass->code[nsym]);

        for (; isalpha(ass->code[nsym]); ++nsym, ++cmd_len); //string is a command

        ass->commands[ncommand].cmd_len = cmd_len;

        for (; ass->code[nsym] == ' '; ++nsym);

        if (ass->code[nsym] == '\n' || ass->code[nsym] == '\0') { //command without arguments
            ass->commands[ncommand].val_ptr = nullptr;

            ++nsym;
            ++ncommand;

            ASS_DEBUG("there is no value for this command.\n");

            continue;
        }

        ass->commands[ncommand].val_ptr = &(ass->code[nsym]); //command with argument
        ASS_DEBUG("number of value's position: %lld\n", nsym);

        parce_command(ass, &ass->commands[ncommand], &position_in_assembled_code);

        nsym += skip_to_newline(&ass->code[nsym]);

        ++nsym;
        ++ncommand;
    }

    ass->amount_of_code_strings = ncommand;
    ass->amount_of_markers      = nmarker;

    return ncommand;
}

int verify_markers(const Marker *markers, int amount_of_markers) {
    assert(markers != nullptr && "nullptr to markers");

    /*verify pointers*/

    for (int i = 0; i < amount_of_markers; ++i) {
        if (markers[i]. ptr == nullptr) {
            return UNEXP_NULLPTR;
        }
    }

    /*checkup for identical markers*/

    for (int i = 0; i < amount_of_markers; ++i) {
        for (int j = i + 1; j < amount_of_markers; ++j) {
            if (markers[i].len == markers[j].len) {
                if (strnicmp(markers[i].ptr, markers[j].ptr, markers[i].len) == 0) {
                    return SAME_MARKERS;
                }
            }
        }
    }

    return NO_ASS_ERR;
}

static Register get_reg_num(char *ptr) {
    if (*ptr != 'r' || *(ptr + 2) != 'x') {
        return NOT_REG;
    }

    int reg_num = *(ptr + 1) - 'a' + 1;

    if (reg_num > MIN_REG && reg_num < MAX_REG) {
        return (Register) reg_num;
    }

    return NOT_REG;
}

static int get_args_with_first_reg(Command *command, int *shift) {
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

static int get_args_with_first_val(Command *command, int *shift) {
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

static size_t skip_comment(char *pointer) {
    assert(pointer != nullptr && "pointer is nullptr");

    size_t i = 0;

    if (*pointer == ';') {
        i += skip_to_newline (pointer + i) + 1;
        i += skip_spaces     (pointer + i);
        i += skip_comment    (pointer + i);
    }

    return i;
}
