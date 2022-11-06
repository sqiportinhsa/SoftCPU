;main
    in
    call fibonacci
    out
    out
    hlt

:fibonacci
    push 1
    sub
    pop rax
    push rax
    push rax
    push 2
    ja n_eq_1_or_2
    call fibonacci
    push 1
    sub
    call fibonacci
    pop rax
    add
    push rax
    push 2
    add
    ret

:n_eq_1_or_2
    pop rax
    push 1
    push rax
    push 1
    add
    ret