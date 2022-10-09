#include "stack_logs.h"

#include <stdio.h>
#include <math.h>
#include <stdarg.h>

#include "stack_verification.h"

void RealDumpLogs(Stack *stk, const char *file, const char *func, int line, int errors) {
    FILE *logs = nullptr;

    #ifdef LOGS_TO_FILE

    logs = fopen("logs.txt", "a");

    #else
    #ifdef LOGS_TO_CONSOLE
    logs = stdout;
    #endif
    #endif

    if (logs == nullptr) {
        return;
    }

    if (stk == nullptr) {
        Print(logs, "Can't print logs: pointer to stack is crushed");
        return;
    }

    Print(logs, "Logs called in %s at %s(%d):\n", func, file, line);

    if (ErrorIsThere(errors, LOGS_PTR_CRASHED) || stk->logs == nullptr) {
        Print(logs, "LOGS_PTR_CRASHED: information about creation of stack was lost.\n");

    } else {
        if (ErrorIsThere(errors, FILE_INF_CRASHED)) {
            Print(logs, "FILE_INFO_CRASHED: cannot find information about file of creation\n");

            if (ErrorIsThere(errors, FUNC_INF_CRASHED)) {
                Print(logs, "FUNC_INFO_CRASHED:");
                Print(logs, "cannot find information about function of creation\n");
            } else {
                Print(logs, "Stack [%p] created in function %s line %d.\n", &stk, 
                        stk->logs->func_of_creation, stk->logs->line_of_creation);
            }

        } else if (ErrorIsThere(errors, FUNC_INF_CRASHED)) {
            Print(logs, "Stack [%p] created in file %s (line: %d).\n", &stk, 
                    stk->logs->file_of_creation, stk->logs->line_of_creation);
            Print(logs, "FUNC_INFO_CRASHED:");
            Print(logs, "cannot find information about function of creation\n");
            
        } else {
            Print(logs, "Stack [%p] created at %s(%d) in function %s.\n", &stk, 
                    stk->logs->file_of_creation, stk->logs->line_of_creation, 
                    stk->logs->func_of_creation);
        }
    }

    if (ErrorIsThere(errors, SIZE_EXCEED_CAP)) {
        Print(logs, "Error: size exceed capacity\n");
    }

    #ifdef HASH_VERIFICATION
    if (ErrorIsThere(errors, HASH_DISMATCH)) {
        Print(logs, "Error: Hash dismatch. Data may be lost.");
    }
    #endif

    Print(logs, "Stack info:\n");

    Print(logs, "{\n");
    Print(logs, "\t capacity = %zd\n", stk->capacity);
    Print(logs, "\t size     = %zd\n", stk->size);
    Print(logs, "\t data [%p]\n",       stk->data);

    if (ErrorIsThere(errors, UNEXPECTED_PSN)) {
        Print(logs, "\t Data error: unexpected poison in element's cell\n");
    }
    if (ErrorIsThere(errors, UNEXPECTED_ELM)) {
        Print(logs, "\t Data error: element in poisoned cell\n");
    }

    if (ErrorIsThere(errors, DATA_PTR_CRASHED)) {
        Print(logs, "DATA_PTR_CRASHED: cannot print data, information was lost\n");
    } else {
        Print(logs, "\t {\n");

        #ifdef CANARY_VERIFICATION
        Canary_t *l_border_ptr = (Canary_t*) ((char*)stk->data - sizeof(Canary_t));
        Canary_t *r_border_ptr = (Canary_t*) ((char*)stk->data + sizeof(Elem_t) * stk->capacity);

        if (ErrorIsThere(errors, L_BORDER_CHANGED)) {
            Print(logs, "\t \t Left  Border = %zd (expected %zd)\n", *l_border_ptr, Border);
            Print(logs, "errors: %d\n", errors);
        } else {
            Print(logs, "\t \t Left  Border = %zd (OK)\n", *l_border_ptr);
        }
        #endif

        for (size_t i = 0; i < stk->size && i < stk->capacity; ++i) {
            Print(logs, "\t \t*[%zd] = " PFORMAT,   i, stk->data[i]);
            #ifdef DEEP_VERIFICATION
            if (IsPoisoned(stk->data[i])) {
                Print(logs, " (poisoned)");
            } else {
                Print(logs, " (busy)");
            }
            #endif
            Print(logs, "\n");
        }


        for (size_t i = stk->size; i < stk->capacity; ++i) {
            Print(logs, "\t \t [%zd] = " PFORMAT, i, stk->data[i]);
            #ifdef DEEP_VERIFICATION
            if (IsPoisoned(stk->data[i])) {
                Print(logs, " (poisoned)");
            } else {
                Print(logs, " (busy)");
            }
            #endif
            Print(logs, "\n");
        }

        #ifdef CANARY_VERIFICATION
        if (ErrorIsThere(errors, R_BORDER_CHANGED)) {
            Print(logs, "\t \t Right Border = %zd (expected %llu)\n", *r_border_ptr, Border);
        } else {
            Print(logs, "\t \t Right Border = %zd (OK)\n", *r_border_ptr);
        }
        #endif

        Print(logs, "\t }\n");
    }

    Print(logs, "}\n\n");

    #ifdef LOGS_TO_FILE
    fclose(logs);
    #endif
}

void Print(FILE *output, const char *format, ...) {
    va_list ptr = {};
    va_start(ptr, format);

    if (output == nullptr) {
        return;
    }

    #ifdef LOGS_TO_FILE

    vfprintf(output, format, ptr);

    #endif

    #ifdef LOGS_TO_CONSOLE
    #ifdef LOGS_TO_FILE

    vfprintf(stdout, format, ptr);

    #else

    vfprintf(output, format, ptr);

    #endif
    #endif

    va_end(ptr);
}
