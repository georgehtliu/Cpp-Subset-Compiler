.import print
lis $10
.word print
.import init
.import new
.import delete
beq $0, $0, wain
wain:
lis $11
.word 1
lis $4
.word 4
sub $29, $30, $4  ; setup frame pointer
sw $1, -4($30)
sub $30, $30, $4
sw $2, -4($30)
sub $30, $30, $4
lis $5
.word 1
sw $5, -4($30)
sub $30, $30, $4
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word init
jalr $5
add $30, $30, $4
lw $31, -4($30)
lw $3, -4($29)
add $30, $30, $4
add $30, $30, $4
add $30, $30, $4
jr $31