#include <stdio.h>
#include <cassert>

#include "utils.hpp"
#include "file_reading.hpp"

int get_commands(char *text, Command *commands, size_t amount_of_elems) {
    assert(text     != nullptr && "text is nullptr");
    assert(commands != nullptr && "commands is nullptr");
    
    int n_command = 0;
    size_t n_elem = 0;

    while (n_elem < amount_of_elems) {
        int cmd = 0;

        //printf("ptr before:                  %lld\n", n_elem);

        n_elem += get_val(&(text[n_elem]), &cmd);

        //printf("ptr after getting a command: %lld\n", n_elem);

        commands[n_command].cmd = get_cmd(cmd);

        ++n_elem;

        //printf("ptr to next symbol:          %lld\n", n_elem);

        switch (commands[n_command].cmd) {
            case CMD_PUSH:
                n_elem += get_val(&(text[n_elem]), &(commands[n_command].val));
                ++n_elem;
                //printf("ptr to sym after value:      %lld\n", n_elem);
                break;
            default:
                commands[n_command].val = 0;
        }

        //printf("%d ",  commands[n_command].val);
        //printf("%d\n", commands[n_command].cmd);
        ++n_command;
    }

    return n_command;
}

Commands get_cmd(int val) {
    return (val > MIN_CMD && val < MAX_CMD) ? (Commands) val : NOT_A_CMD;
}
