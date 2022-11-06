in
push 100
mul
pop rax

in
push 100
mul
pop rbx

in
push 100
mul
pop rcx

;check a for 0
push rax
push 0
je zero_a

;non zero a
call calc_discr
push 0
push rdx
ja  two_roots
push 0
push rdx
je  one_root
jmp no_roots

:zero_a
push rbx
push 0
je zero_b

;linear eq
push -1
push rcx 
push rbx 
div
mul
push 1
out
out
hlt

:zero_b
push rcx
push 0
je inf_roots
jmp no_roots

:no_roots
push 0
out
hlt

:inf_roots
push -1
out
hlt

:one_root
push 1
out
push -1
push rbx
push rax
push 2
mul
div
mul
out
hlt


:two_roots
push 2
out
push -1
push rbx
mul
push rdx
sqrt
add
push 2
push rax
mul
div
out
push -1
push rbx
mul
push rdx
sqrt
sub
push 2
push rax
mul
div
out
hlt


:calc_discr
push rbx
push rbx
mul
push 4
push rax
push rcx
mul
mul
sub
pop rdx
ret