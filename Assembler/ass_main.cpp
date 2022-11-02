#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ass.hpp"
#include "../Stack/stack.h"
#include "../Stack/stack_logs.h"
#include "../Stack/stack_verification.h"
#include "../Common/file_reading.hpp"


int main(int argc, const char **argv) {
    int errors = NO_ASS_ERR;

    CLArgs args = cmd_line_args(argc, argv);

    Ass ass = {};


    errors |= ass_constructor(&ass, args.input);

    if (errors != 0) {
        return -1;
    }

    errors |= verify_markers(ass.markers, ass.amount_of_markers);

    errors |= assemble(&ass);

    free_ass(&ass);
    
    return 0;
}