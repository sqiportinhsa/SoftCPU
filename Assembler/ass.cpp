#include <string.h>
#include <ctype.h>
#include <cassert>

#include "ass.hpp"
#include "../Common/common.hpp"
#include "../Common/file_reading.hpp"


static Register get_reg_num(char *ptr);

static int first_ass_pass(Ass *ass);
static int second_ass_pass(Ass *ass, FILE *output);

static size_t process_marker  (Ass *ass, size_t nsym, int position_in_assembled_code, 
                                                               unsigned int nmarker);
static size_t process_command (Ass *ass, unsigned int ncommand, size_t nsym, int cmd_len);

static void write_command(FILE *output, const Command *command);

static int parse_command(Ass *ass, Command *command, int *index_in_assembled_code = nullptr);

static int verify_markers(const Marker *markers, unsigned int amount_of_markers);

static int parse_for_pop  (Command *command, int cmd_num);
static int parse_for_push (Command *command, int cmd_num);
static int parse_for_jump (Command *command, int cmd_num, Ass *ass);

static int get_ram_arg(Command *command, int shift);

static int get_args_with_first_reg(Command *command, int *shift);
static int get_args_with_first_val(Command *command, int *shift);

static size_t skip_comment(char *pointer);
static size_t skip_spaces_and_comments(char *pointer);


const char *default_input_filename  = "input.asm";
const char *default_output_filename = "output.bin";

const int buf_size = 4096;


CLArgs cmd_line_args(int argc, const char **argv) {
    assert(argv != nullptr);

    CLArgs args = parse_cmd_line(argc, argv);

    if (args.input == nullptr) {
        args.input  = default_input_filename;
    }

    if (args.output == nullptr) {
        args.output = default_output_filename;
    }

    return args;
}

int ass_constructor(Ass *ass, CLArgs *args) {
    assert(args != nullptr && "nullptr to filname");
    assert(args != nullptr && "nullptr to args");

    ass->amount_of_code_symbols = count_elements_in_file(args->input);

    ass->code = (char*) calloc(ass->amount_of_code_symbols, sizeof(char));

    if (ass->code == nullptr) {

        return MEMORY_EXCEED; 
    }


    ass->amount_of_code_symbols = read_file(ass->code, ass->amount_of_code_symbols, args->input);

    ass->amount_of_code_strings = (unsigned int) count_strings(ass->code, 
                                                               ass->amount_of_code_symbols);


    ass->commands = (Command*) calloc((size_t) ass->amount_of_code_strings, sizeof(Command));

    ass->markers  = (Marker*)  calloc((size_t) ass->amount_of_code_strings, sizeof(Marker));


    if (ass->commands == nullptr || ass->markers == nullptr) {

        free_ass(ass);

        return MEMORY_EXCEED;
    }

    ass->input_name  = args->input;
    ass->output_name = args->output;

    return 0;
}

void free_ass(Ass *ass) {
    assert(ass != nullptr);

    free(ass->code);
    free(ass->commands);
    free(ass->markers);

    ass->code     = nullptr;
    ass->commands = nullptr;
    ass->markers  = nullptr;
}

int assemble(Ass *ass) {
    if (ass == nullptr) {
        return UNEXP_NULLPTR;
    }

    if (ass->commands == nullptr || ass->markers == nullptr || ass->code == nullptr) {
        return UNEXP_NULLPTR;
    }

    int errors   = 0;

    FILE *output = fopen(ass->output_name, "wb");
    setvbuf(output, nullptr, _IOFBF, buf_size);

    if (output == nullptr) {
        fprintf(stderr, "error: can't open file for output");
        errors |= CANT_OPEN_FILE;
        return errors;
    }

    errors |= first_ass_pass(ass);

    RETURN_IF_FOR_ASSEMBLE(errors);

    errors |= verify_markers(ass->markers, ass->amount_of_markers);

    RETURN_IF_FOR_ASSEMBLE(errors);

    ASS_DEBUG("\n\n--------------------------------------------------\n\n");

    fwrite(&Verification_const,       sizeof(int), 1, output);
    fwrite(&Ass_version,              sizeof(int), 1, output);
    fwrite(&ass->amount_of_commands,  sizeof(int), 1, output);

    errors |= second_ass_pass(ass, output);

    RETURN_IF_FOR_ASSEMBLE(errors);

    ASS_DEBUG("all done");

    fclose(output);

    return NO_ASS_ERR;
}

static int first_ass_pass(Ass *ass) {
    if (ass->code == nullptr || ass->commands == nullptr) {
        return UNEXP_NULLPTR;
    }

    ass->commands[0].cmd_ptr = &(ass->code[0]);

    int errors   = 0;

    unsigned int ncommand = 0;
    unsigned int  nmarker = 0;

    size_t  nsym = 0;

    int position_in_assembled_code = 3 * sizeof(int);

    while (ncommand < ass->amount_of_code_strings && 
            nmarker < ass->amount_of_code_strings &&
               nsym < ass->amount_of_code_symbols) {

        int cmd_len = 0;

        ASS_DEBUG("start\n");

        nsym += skip_spaces_and_comments(ass->code + nsym);

        if (ass->code[nsym] == '\0') {
            break;
        }

        if (ass->code[nsym] == ':') { //string is a marker
            nsym = process_marker(ass, nsym, position_in_assembled_code, nmarker);

            ASS_DEBUG("index processing marker: %zu\n", nsym);

            ++nmarker;

            continue;
        }

        nsym = process_command(ass, ncommand, nsym, cmd_len);

        errors |= parse_command(ass, &ass->commands[ncommand], &position_in_assembled_code);
        
        RETURN_IF(errors);

        ++nsym;
        ++ncommand;

        if (ncommand % 1000 == 0) {
            printf("loading... parsed command %d\n", (int) ncommand);
        }
    }

    ass->amount_of_code_strings = ncommand;
    ass->amount_of_markers      = nmarker;
    ass->amount_of_commands     = ncommand;

    return NO_ASS_ERR;
}

static size_t process_marker(Ass *ass, size_t nsym, int position_in_assembled_code, 
                                                              unsigned int nmarker) {

    assert(ass != nullptr);
    
    ++nsym;
            
    ass->markers[nmarker].ptr                = &(ass->code[nsym]);
    ass->markers[nmarker].index_in_assembled = (unsigned int) position_in_assembled_code;
    ass->markers[nmarker].len               += (unsigned int) skip_to_newline(ass->code + nsym);

    nsym += ass->markers[nmarker].len;

    ++nsym;

    return nsym;
}

static size_t process_command(Ass *ass, unsigned int ncommand, size_t nsym, int cmd_len) {
    assert(ass != nullptr);
    
    ASS_DEBUG("index of command first symbol: %zu. First symbol: %c\n", nsym, ass->code[nsym]);

    ass->commands[ncommand].cmd_ptr = &(ass->code[nsym]);

    for (; isalpha(ass->code[nsym]); ++nsym, ++cmd_len);

    ASS_DEBUG("index of first space after command: %zu\n", nsym);

    ass->commands[ncommand].cmd_len = cmd_len;

    ASS_DEBUG("command len: %d\n", ass->commands[ncommand].cmd_len);

    for (; ass->code[nsym] == ' '; ++nsym);

    ASS_DEBUG("index after skipping spaces: %zu\n", nsym);

    if (ass->code[nsym] == '\n' || ass->code[nsym] == '\0') { //command without arguments
        ass->commands[ncommand].val_ptr = nullptr;

        ASS_DEBUG("there is no value for this command.\n");

    } else { //command with argument
        ass->commands[ncommand].val_ptr = &(ass->code[nsym]);

        ASS_DEBUG("number of value's position: %zu\n", nsym);
    }

    nsym += skip_to_newline(&ass->code[nsym]);

    return nsym;
}

static int second_ass_pass(Ass *ass, FILE *output) {
    if (ass->code == nullptr || ass->commands == nullptr) {
        return UNEXP_NULLPTR;
    }

    for (unsigned int ncommand = 0; ncommand < ass->amount_of_commands; ++ncommand) {
        ASS_DEBUG("working with command number %d, command code: %d\n", ncommand, 
                                                 ass->commands[ncommand].cmd.cmd);

        parse_command(ass,    &ass->commands[ncommand]);
        write_command(output, &ass->commands[ncommand]);

        if (ncommand % 1000 == 0) {
            printf("loading... parsed command %d\n", (int) ncommand);
        }
    }
    
    return NO_ASS_ERR;
}

static void write_command(FILE *output, const Command *command) {

    assert(output  != nullptr);
    assert(command != nullptr);

    ASS_DEBUG("\nstart processing command. command number: %d\n", command->cmd.cmd);

    fwrite(&command->cmd, sizeof(CMD), 1, output);


    if (command->cmd.val) {

        ASS_DEBUG("there is a value for this command: %d\n", command->val);

        fwrite(&command->val, sizeof(int), 1, output);
    }

    if (command->cmd.reg) {

        ASS_DEBUG("there is a register for this command: %d\n", command->reg);

        fwrite(&command->reg, sizeof(char), 1, output);
    }
}

#define DEF_CMD(name, cmd_num, args, ...)                                                          \
    if (strcasecmp(cmd, #name) == 0) {                                                             \
                                                                                                   \
        if (args == NO_ARGS) {                                                                     \
                                                                                                   \
            command->cmd.cmd = cmd_num;                                                            \
                                                                                                   \
            if (command->val_ptr != nullptr) {return UNEXPEC_ARG;}                                 \
                                                                                                   \
        } else if (args == POP__ARGS) {                                                            \
                                                                                                   \
            errors |= parse_for_pop(command, cmd_num);                                             \
                                                                                                   \
            if (errors != 0) {return errors;}                                                      \
                                                                                                   \
        } else if (args == PUSH_ARGS) {                                                            \
                                                                                                   \
            errors |= parse_for_push(command, cmd_num);                                            \
                                                                                                   \
            if (errors != 0) {return errors;}                                                      \
                                                                                                   \
        } else if (args == JUMP_ARGS) {                                                            \
                                                                                                   \
            errors |= parse_for_jump(command, cmd_num, ass);                                       \
                                                                                                   \
            if (errors != 0) {return errors;}                                                      \
        }                                                                                          \
                                                                                                   \
    } else

static int parse_command(Ass *ass, Command *command, int *index_in_assembled_code) {
    if (ass == nullptr || command == nullptr) {
        return UNEXP_NULLPTR;
    }

    int errors = NO_ASS_ERR;

    int len = command->cmd_len;

    char cmd[Max_cmd_len] = {};

    memcpy(cmd, command->cmd_ptr, (size_t) len);

    cmd[len] = '\0';

    command->cmd.cmd = 0;
    command->val     = 0;
    command->reg     = 0;

    #include "../Common/commands.hpp"
    /*else*/ {
        fprintf(stderr, "invalid command: %s\n", cmd);
    
        return INCORRECT_CMD;
    }

    int cmd_size = sizeof(char);

    if (command->cmd.val) {

        cmd_size += (int) sizeof(int);
    }

    if (command->cmd.reg) {

        cmd_size += (int) sizeof(char);
    }

    if (index_in_assembled_code != nullptr) {

        *index_in_assembled_code += cmd_size;
    }
    
    return errors;
}

#undef DEF_CMD

static int parse_for_pop(Command *command, int cmd_num) {
    assert (command != nullptr);

    int errors = 0;

    command->cmd.cmd = cmd_num;                                                                    
                                                                                           
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
                                                                                           
        if (command->cmd.val) {                                                          
                                     
            errors |= INCORRECT_VAL;                                                                                                                      
        }      

        RETURN_IF(errors);                                                                            
                                                          
        command->cmd.reg = 1;                                                               
                                                                                           
    } else if (*(command->val_ptr + 1) == 'a' &&                                           
               *(command->val_ptr + 2) == 'm' &&                                           
               *(command->val_ptr + 3) == '[') {                                           
                                                                                           
        shift += (int) sizeof("ram[") - 1;                                                                        
                                                                                           
        get_ram_arg(command, shift);

        RETURN_IF(errors);
                                                                                           
        command->cmd.ram = 1;
    }

    return errors;
}
      
static int parse_for_push(Command *command, int cmd_num) {

    assert(command != nullptr);
    
    int errors = 0;

    command->cmd.cmd = cmd_num;
                                                                                                   
    if (command->val_ptr == nullptr) {
        fprintf(stderr, "incorrect push: argument expected\n");
        return MISSING_ARG;
    }

    int shift = 0;
                                                                                                   
    if (isdigit(*command->val_ptr) || *command->val_ptr == '-') {
                                                                                                   
        errors |= get_args_with_first_val(command, &shift);
                                                                                                   
    } else if (*command->val_ptr == 'r') {
                                                                                                   
        errors |= get_args_with_first_reg(command, &shift);
                                                                                               
        if (command->cmd.val) {
                                                                                               
            errors |= INCORRECT_VAL;
        }
                                                                                               
    } else if (*command->val_ptr == '[') {
        ++shift;
                                                                                               
        errors |= get_ram_arg(command, shift);

        RETURN_IF(errors);
                                                                                               
        command->cmd.ram = 1;
                                                                                               
    } else {

        fprintf(stderr, "incorrect push argument, symbol '%c'\n", *command->val_ptr);
                                                                                               
        errors |= INCORRECT_ARG;
    }

    return errors;
}

static int parse_for_jump(Command *command, int cmd_num, Ass *ass) {
    assert (command          != nullptr);
    assert (ass              != nullptr);
    assert (ass->markers     != nullptr);
    assert (command->val_ptr != nullptr);

    command->cmd.cmd = cmd_num;
    command->cmd.val = 1;

    for (unsigned int nmarker = 0; nmarker < ass->amount_of_markers; ++nmarker) {

        if (strncasecmp(ass->markers[nmarker].ptr, command->val_ptr, 
                                    ass->markers[nmarker].len) == 0) {

            command->val = (int) ass->markers[nmarker].index_in_assembled;

            ASS_DEBUG("index in assembled: %d\n", command->val);
            
            break;
        }
    }

    return NO_ASS_ERR;
}


static int get_ram_arg(Command *command, int shift) {
    assert(command != nullptr);

    int errors  = NO_ASS_ERR;
    int shift_before = shift;

    errors |= get_args_with_first_val(command, &shift);
    errors |= get_args_with_first_reg(command, &shift);

    if (shift == shift_before) {                                                                  
                                                                                           
        fprintf(stderr, "incorrect command: argument in [] expected\n");                   
                                                                                           
        errors |= INCR_BR_USAGE;                                                       
                                                                                           
        return errors;                                                                 
    }                                                                                  
                                                                                           
    if (*(command->val_ptr + shift) != ']') {
                                                                                           
        fprintf(stderr, "incorrect command argument: ] expected\n");
                                                                                           
        errors |= INCR_BR_USAGE;
                                                                                           
        return errors;
    }

    return NO_ASS_ERR;
}

static int verify_markers(const Marker *markers, unsigned int amount_of_markers) {
    assert(markers != nullptr && "nullptr to markers");

    /*verify pointers*/

    for (unsigned int i = 0; i < amount_of_markers; ++i) {
        if (markers[i]. ptr == nullptr) {
            return UNEXP_NULLPTR;
        }
    }

    /*checkup for identical markers*/

    for (unsigned int i = 0; i < amount_of_markers; ++i) {
        for (unsigned int j = i + 1; j < amount_of_markers; ++j) {
            if (markers[i].len == markers[j].len) {
                if (strncmp(markers[i].ptr, markers[j].ptr, markers[i].len) == 0) {
                    return SAME_MARKERS;
                }
            }
        }
    }

    return NO_ASS_ERR;
}

static Register get_reg_num(char *ptr) {
    assert(ptr != nullptr);

    if (*ptr != 'r' || *(ptr + 2) != 'x') {
        return NOT_REG;
    }

    int reg_num = *(ptr + 1) - 'a' + 1;

    if (reg_num > MIN_REG && reg_num < MAX_REG) {
        return (Register) reg_num;
    }

    return NOT_REG;
}

static int get_args_with_first_reg(Command *command, int *shift) {  //Velosiped (c) Mentor
    assert(command != nullptr);
    assert(shift   != nullptr);
    
    int errors = NO_ASS_ERR;

    if (*(command->val_ptr + *shift) == 'r') {
        command->reg = get_reg_num(command->val_ptr + *shift);
        if (command->reg == -1) {
            fprintf(stderr, "incorrect push arguments: invalid register\n");
            errors |= INCORRECT_REG;
            return errors;
        }

        command->cmd.reg = 1;

        *shift += 3;
        *shift += (int) skip_spaces(command->val_ptr + *shift);

        if (*(command->val_ptr + *shift) != '\n' && 
            *(command->val_ptr + *shift) != ';'  &&
            *(command->val_ptr + *shift) != ']') {

            if (*(command->val_ptr + *shift) != '+') {
                fprintf(stderr, "incorrect push arguments: invalid symbol '%c' aka '%d'"
                   " after register\n", *(command->val_ptr + *shift), *(command->val_ptr + *shift));

                errors |= UNIDENTIF_SYM;
                return errors;
            }

            ++(*shift);
            *shift += (int) skip_spaces(command->val_ptr + *shift);

            if (isdigit(*(command->val_ptr + *shift)) || *(command->val_ptr + *shift) == '-') {
                *shift += get_val(command->val_ptr + *shift, &command->val);
                command->cmd.val = 1;

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
    assert(command != nullptr);
    assert(shift   != nullptr);

    int errors = NO_ASS_ERR;

    if (isdigit(*(command->val_ptr + *shift)) || *(command->val_ptr + *shift) == '-') {

        *shift += get_val(command->val_ptr + *shift, &command->val);
        *shift += (int) skip_spaces(command->val_ptr + *shift);

        command->cmd.val = 1;

        if (*(command->val_ptr + *shift) != '\n' && 
            *(command->val_ptr + *shift) != ';'  &&
            *(command->val_ptr + *shift) != ']') {

            if (*(command->val_ptr + *shift) != '+') {
                fprintf(stderr, "incorrect push arguments: unexpected symbol '%c' aka '%d' "
                       "after value\n", *(command->val_ptr + *shift), *(command->val_ptr + *shift));
                errors |= UNIDENTIF_SYM;
                return errors;
            }

            ++(*shift);
            *shift += (int) skip_spaces(command->val_ptr + *shift);

            command->reg = get_reg_num(command->val_ptr + *shift);
            if (command->reg == -1) {
                fprintf(stderr, "incorrect push arguments: invalid register addition\n");
                errors |= INCORRECT_REG;
                return errors;
            }

            command->cmd.reg = 1;
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

static size_t skip_spaces_and_comments(char *pointer) {
    assert (pointer != nullptr && "pointer is nullptr");

    size_t i = 0;

    for (; *(pointer + i) == '\n' || *(pointer + i) == ' ' || *(pointer + i) == ';'; ++i) {
        if (*(pointer + i) == ';') {
            i += skip_to_newline(pointer + i);
        }
    }

    return i;
}
