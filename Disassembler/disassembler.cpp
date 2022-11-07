#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cerrno>

#include "disassembler.hpp"
#include "../Common/file_reading.hpp"

static bool binary_is_ok(Disasm *disasm);

static void print_args(CMD cmd, Disasm *disasm, FILE *output);

static bool marker_for_ip(size_t ip, int *markers, int markers_amount);

const char *default_input_filename  = "output.bin";
const char *default_output_filename = "output.asm";


#define DEF_CMD(name, num, args, ...)                                  \
        case num :                                                     \
            fprintf(output, "%s ", #name);                             \
            if (args != NO_ARGS) {                                     \
                print_args(cmd, disasm, output);                       \
            }                                                          \
                                                                       \
            fprintf(output, "\n");                                     \
            break;                                         

void disassemble(Disasm *disasm, const char *output_name) {
    FILE *output = fopen(output_name, "w");

    if (output == nullptr) {
        fprintf(stderr, "can't open output file %s\n", output_name);
    }

    int n_command = 0;

    while (n_command < disasm->amount_of_cmds && disasm->ip < disasm->code_len - 1) {

        CMD cmd = *((CMD*) (disasm->code + disasm->ip));

        if (marker_for_ip(disasm->ip, disasm->markers, disasm->amount_of_markers)) {
            fprintf(output, ":%zu\n", disasm->ip);
        }

        disasm->ip += sizeof(char);

        switch (cmd.cmd) {
            #include "../Common/commands.hpp"
            default:
                return;
        }

        ++n_command;
        fflush(output);
    }

    fclose(output);
}

#undef DEF_CMD

#define DEF_CMD(name, num, args, ...)                                        \
        case num:                                                            \
            if (args == JUMP_ARGS) {                                         \
                disasm->markers[n_marker] = *((int*) (disasm->code + ip));   \
                ++n_marker;                                                  \
                                                                             \
                ip += (int) sizeof(int);                                     \
            }                                                                \
                                                                             \
            break;

bool get_markers(Disasm *disasm) {
    int    n_command = 0;
    int    n_marker  = 0;
    size_t ip        = disasm->ip;

    while (n_command < disasm->amount_of_cmds && ip < disasm->code_len) {
        
        CMD cmd = *((CMD*) (disasm->code + ip));

        ip += sizeof(char);

        switch (cmd.cmd) {
            #include "../Common/commands.hpp"
            default:
                return false;
        }

        ++n_command;
    }

    disasm->amount_of_markers = n_marker;

    return true;
}

#undef DEF_CMD

bool init_disasm(Disasm *disasm, const char *input_name) {
    assert(disasm != nullptr);

    size_t code_len = count_elements_in_file(input_name);

    disasm->code = (char*) calloc(code_len, sizeof(char));

    if (disasm->code == nullptr) {
        fprintf(stderr, "Error: not enough memory for code\n");
        return false;
    }

    disasm->code_len = read_file(disasm->code, code_len, input_name);

    if (!binary_is_ok(disasm)) {
        return false;
    }

    disasm->code_len = code_len;

    disasm->markers = (int*) calloc(disasm->amount_of_cmds, sizeof(int));

    if (disasm->markers == nullptr) {
        fprintf(stderr, "Error: not enough memory for markers array\n");
        return false;
    }

    return true;
}

CLArgs get_cmd_line_args(int argc, const char **argv) {
    CLArgs clargs = parse_cmd_line(argc, argv);

    if (clargs.input == nullptr) {
        clargs.input = default_input_filename;
    }

    if (clargs.output == nullptr) {
        clargs.output = default_output_filename;
    }

    return clargs;
}

void free_disasm(Disasm *disasm) {
    free(disasm->markers);
    free(disasm->code);
}

static bool binary_is_ok(Disasm *disasm) {

    disasm->verification_const = *((int*) (disasm->code + disasm->ip));
    disasm->ip += sizeof(int);
    
    if (disasm->verification_const != Verification_const) {
        fprintf(stderr, "Error: incorrect binary file, verification constant doesn't match."
                        " Expected: %d got: %d\n", Verification_const, disasm->verification_const);
        return false;
    }

    disasm->ass_version = *((int*) (disasm->code + disasm->ip));
    disasm->ip += sizeof(int);

    if (disasm->ass_version > Disasm_version) {
        fprintf(stderr, "Error: processor version is less than assembler version\n");
        return false;
    }

    disasm->amount_of_cmds = *((int*) (disasm->code + disasm->ip));
    disasm->ip += sizeof(int);

    return true;
}

static void print_args(CMD cmd, Disasm *disasm, FILE *output) {
    int  cmd_val = 0;
    char cmd_reg = 0;

    if (cmd.val) {
        cmd_val = *((int*)  (disasm->code + disasm->ip));
        disasm->ip += sizeof(int);
    }

    if (cmd.reg) {
        cmd_reg = *((char*) (disasm->code + disasm->ip));
        disasm->ip += sizeof(char);
    }

    if (cmd.ram) {
        fprintf(output, "[");
    }
    
    if (cmd.reg) {
        fprintf(output, "r%cx", 'a' - cmd_reg + 1);
    }

    if (cmd.reg && cmd.val) {
        fprintf(output, " + ");
    }

    if (cmd.val) {
        fprintf(output, "%d", cmd_val);
    }

    if (cmd.ram) {
        fprintf(output, "]");
    }
}

static bool marker_for_ip(size_t ip, int *markers, int markers_amount) {
    for (int i = 0; i < markers_amount; ++i) {
        if (markers[i] == (int) ip) {
            return true;
        }
    }

    return false;
}
