#define No_args 0
#define Standart_args 1
#define Jump_args 2

DEF_CMD(HLT, 0, No_args)

DEF_CMD(PUSH, 1, Standart_args, {
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

DEF_CMD(ADD, 2, No_args, {
    cpu->stk_err |= StackPush(cpu->cpu_stack, StackPop(cpu->cpu_stack) 
                                            + StackPop(cpu->cpu_stack));
})

DEF_CMD(MUL, 3, No_args, {
    cpu->stk_err |= StackPush(cpu->cpu_stack, StackPop(cpu->cpu_stack) 
                                            * StackPop(cpu->cpu_stack));
})

DEF_CMD(SUB, 4, No_args, {
    first_popped  = StackPop (cpu->cpu_stack, &cpu->stk_err);
    cpu->stk_err |= StackPush(cpu->cpu_stack, StackPop(cpu->cpu_stack) - first_popped);
})

DEF_CMD(DIV, 5, No_args, {
    first_popped  = StackPop (cpu->cpu_stack, &cpu->stk_err);
    cpu->stk_err |= StackPush(cpu->cpu_stack, StackPop(cpu->cpu_stack) / first_popped);
})

DEF_CMD(OUT, 6, No_args, {
    printf("result: %d", StackPop(cpu->cpu_stack, &cpu->stk_err));
})

DEF_CMD(POP, 7, Standart_args, {
    if (cmd & REG) {
        cpu->registers[(int) cmd_reg] = StackPop(cpu->cpu_stack, &cpu->stk_err);
    } else if (cmd & RAM) {
        cpu->ram[(int) cmd_val]       = StackPop(cpu->cpu_stack, &cpu->stk_err);
    } else {
        cpu_err |= INCORR_POP;
        return cpu_err;
    }
})

DEF_CMD(IN,  8, No_args, {

})

DEF_CMD(JMP, 9, Jump_args, {

})

#undef No_args
#undef Standart_args
#undef Jump_args