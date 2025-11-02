sw $31, -4($30)
lis $31
.word 4
sub $30, $30, $31
lis $10
.word print
lis $1
.word 57
jalr $10
lis $1
.word -241
jalr $10
lis $1
.word 404
jalr $10
lis $31
.word 4
add $30, $30, $31
lw $31, -4($30)
jr $31