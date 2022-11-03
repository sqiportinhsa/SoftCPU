#include <stdio.h>
#include <stdlib.h>

#include "processor.hpp"
#include "../Common/file_reading.hpp"
#include "../Stack/stack_logs.h"

int main(int argc, const char **argv) {
    FILE *logfile = fopen("logfile.txt", "w");
    SetLogStream(logfile);

    int errors = NO_CPU_ERR;

    CPU cpu = {};

    errors |= prepare_cpu(&cpu, argc, argv);

    RETURN_IF(errors);

    errors |= calculate(&cpu);

    RETURN_IF(errors);

    errors |= CPU_destructor(&cpu);

    RETURN_IF(errors);

    fclose(logfile);

    return 0;
}