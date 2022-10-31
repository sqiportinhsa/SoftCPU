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

DEF_CMD(ADD, 2, NO_ARGS, {
    cpu->stk_err |= StackPush(cpu->cpu_stack, StackPop(cpu->cpu_stack) 
                                            + StackPop(cpu->cpu_stack));
})

DEF_CMD(MUL, 3, NO_ARGS, {
    cpu->stk_err |= StackPush(cpu->cpu_stack, StackPop(cpu->cpu_stack) 
                                            * StackPop(cpu->cpu_stack));
})

DEF_CMD(SUB, 4, NO_ARGS, {
    first_popped  = StackPop (cpu->cpu_stack, &cpu->stk_err);
    cpu->stk_err |= StackPush(cpu->cpu_stack, StackPop(cpu->cpu_stack) - first_popped);
})

DEF_CMD(DIV, 5, NO_ARGS, {
    first_popped  = StackPop (cpu->cpu_stack, &cpu->stk_err);
    cpu->stk_err |= StackPush(cpu->cpu_stack, StackPop(cpu->cpu_stack) / first_popped);
})

DEF_CMD(OUT, 6, NO_ARGS, {
    printf("result: %d", StackPop(cpu->cpu_stack, &cpu->stk_err));
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

DEF_CMD(IN,  8, NO_ARGS, {

})

DEF_CMD(JMP, 9, JUMP_ARGS, {

})

#undef No_args
#undef Standart_args
#undef Jump_args