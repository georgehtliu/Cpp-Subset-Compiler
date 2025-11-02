.import kitten
lis $4
.word 4
sw $31, -4($30)
sub $30, $30, $4
lis $3
.word kitten
jalr $3
add $30, $30, $4
lw $31, -4($30)
jr $31