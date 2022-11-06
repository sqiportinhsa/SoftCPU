#include <stdio.h>
#include <stdlib.h>

#include "disassembler.hpp"
#include "../Common/file_reading.hpp"
#include "../Common/common.hpp"

int main(int argc, const char **argv) {
    CLArgs clargs = get_cmd_line_args(argc, argv);

    Disasm disasm = {};

    if (!init_disasm(&disasm, clargs.input)) {
        fprintf(stderr, "Can't disassemble: disasm inintialisation error\n");
        free_disasm(&disasm);
        return -1;
    }

    if (!get_markers(&disasm)) {
        fprintf(stderr, "Incorrect command, can't disassemble\n");
        free_disasm(&disasm);
        return -1;
    }

    disassemble(&disasm, clargs.output);

    free(&disasm);

    return 0;
}