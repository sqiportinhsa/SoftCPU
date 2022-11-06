#define NO_ARGS 0
#define PUSH_ARGS 1
#define POP__ARGS 2
#define JUMP_ARGS 3

DEF_CMD(HLT, 0, NO_ARGS)

DEF_CMD(PUSH, 1, PUSH_ARGS, {
    if (cmd & VAL) {
        val_for_push += cmd_val;
    }
    if (cmd & REG) {
        val_for_push += cpu->registers[(int) cmd_reg];
    }
    if (cmd & RAM) {
        val_for_push  = cpu->ram[val_for_push];
    }
    cpu->stk_err |= StackPush(cpu->cpu_stack, val_for_push);
})

#define DEF_CMD_ARITHM(name, num, oper, checkup)                                          \
        DEF_CMD(name, num, NO_ARGS, {                                                     \
            cpu->stk_err |= first__popped = StackPop(cpu->cpu_stack);                     \
            cpu->stk_err |= second_popped = StackPop(cpu->cpu_stack);                     \
                                                                                          \
            checkup                                                                       \
                                                                                          \
            cpu->stk_err |= StackPush(cpu->cpu_stack, second_popped oper first__popped);  \
                                                                                          \
            if (cpu->stk_err != 0) {                                                      \
                return cpu_err | STACK_ERR;                                               \
            }                                                                             \
        })

DEF_CMD_ARITHM(ADD, 2, +, ;)
DEF_CMD_ARITHM(MUL, 3, *, ;)
DEF_CMD_ARITHM(SUB, 4, -, ;)

DEF_CMD_ARITHM(DIV, 5, /, {
    if (first__popped == 0) {
        return cpu_err | DIV_BY_NULL;
    }
})

#undef DEF_CMD_ARITHM

DEF_CMD(OUT, 6, NO_ARGS, {
    printf("result: %d\n", StackPop(cpu->cpu_stack, &cpu->stk_err));
})

DEF_CMD(POP, 7, POP__ARGS, {
    if (cmd & REG) {
        cpu->registers[(int) cmd_reg] = StackPop(cpu->cpu_stack, &cpu->stk_err);
    } else if (cmd & RAM) {
        cpu->ram[(int) cmd_val]       = StackPop(cpu->cpu_stack, &cpu->stk_err);
    } else {
        cpu_err |= INCORR_POP;
        return cpu_err;
    }
})

DEF_CMD(JMP, 8, JUMP_ARGS, {
    cpu->ip = *((int *) (cpu->code + cpu->ip) - 1);
})

#define DEF_JMP_IF(name, cmd_num, oper)                                \
        DEF_CMD(name, cmd_num, JUMP_ARGS, {                            \
            first__popped = StackPop (cpu->cpu_stack, &cpu->stk_err);  \
            second_popped = StackPop (cpu->cpu_stack, &cpu->stk_err);  \
                                                                       \
            if (first__popped oper second_popped) {                    \
                cpu->ip = *((int *) (cpu->code + cpu->ip) - 1);        \
            }                                                          \
        })

DEF_JMP_IF(JA,  9 , > )
DEF_JMP_IF(JAE, 10, >=)
DEF_JMP_IF(JB,  11, < )
DEF_JMP_IF(JBE, 12, <=)
DEF_JMP_IF(JE,  13, ==)
DEF_JMP_IF(JNE, 14, !=)

#undef DEF_JMP_IF

DEF_CMD(CALL, 15, JUMP_ARGS, {
    StackPush(cpu->adr_stack, cpu->ip);
    cpu->ip = *((int *) (cpu->code + cpu->ip) - 1);
})

DEF_CMD(RET, 16, NO_ARGS, {
    cpu->ip = StackPop(cpu->adr_stack, &cpu->stk_err);
})

DEF_CMD(IN, 17, NO_ARGS, {
    scanf("%d", &val_for_push);
    cpu->stk_err |= StackPush(cpu->cpu_stack, val_for_push);
})

DEF_CMD(SQRT, 18, NO_ARGS, {
    first__popped  = StackPop(cpu->cpu_stack, &cpu->stk_err);
    if (cpu->stk_err != 0) {
        return cpu_err | STACK_ERR;
    }

    val_for_push  = (int) sqrt((double) first__popped);
    cpu->stk_err |= StackPush(cpu->cpu_stack, val_for_push);
})


#undef No_args
#undef Standart_args
#undef Jump_args