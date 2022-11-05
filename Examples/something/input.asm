push 10
push 20
ADD
out
call lol
jmp br

:lol
push 1337
out
ret 

:br
push 228
out
in
sqrt
out
jmp i_love_foxidokun
hlt

:i_love_foxidokun
PUSH 10
PUSH 20
POP rax
PUSH rax
out
push rax
out
jb 1
ADD
OUT
:aBcd
HLT
:1
PUSH 228
OUT
JMP aBcd