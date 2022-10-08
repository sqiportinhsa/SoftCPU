#ifndef STACK_LOGS
#define STACK_LOGS

#define LOGS_TO_FILE
//#define LOGS_TO_CONSOLE

#include "stack.h"
#include "stack_verification.h"

#define DumpLogs(stk) RealDumpLogs(stk, __FILE__, __PRETTY_FUNCTION__, __LINE__, \
                                                          StackVerificator(stk))

void RealDumpLogs(Stack *stk, const char *file, const char *func, int line, int errors);
void Print(FILE *logs, const char *format, ...);

#endif
