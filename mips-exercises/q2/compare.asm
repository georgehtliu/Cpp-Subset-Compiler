compare:

; prologue
sw $1, -4($30)
sw $2, -8($30)
sw $31, -12($30)
lis $14
.word 12
sub $30, $30, $14

; actual work
slt $3, $1, $2

; epilogue
lis $14
.word 12
add $30, $30, $14
lw $1, -4($30)
lw $2, -8($30)
lw $31, -12($30)
jr $31
