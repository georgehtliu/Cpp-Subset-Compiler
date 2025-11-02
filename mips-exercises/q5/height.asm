height:

heightPrologue:
sw $31, -4($30)
sw $11, -8($30)
sw $21, -12($30)
sw $4, -16($30)
sw $9, -20($30)
sw $10, -24($30)
sw $12, -28($30)
sw $18, -32($30)

lis $11
.word 1
lis $21
.word -1
lis $4
.word 4

lis $31
.word 32

sub $30, $30, $31

main:

add $10, $0, $0
lis $18
.word dfs

jalr $18
add $3, $0, $12

heightEpilogue:
lis $31
.word 32
add $30, $30, $31
lw $31, -4($30)
lw $11, -8($30)
lw $21, -12($30)
lw $4, -16($30)
lw $9, -20($30)
lw $10, -24($30)
lw $12, -28($30)
lw $18, -32($30)
jr $31

dfs:
; index of node ($10), returns in $12

dfsPrologue:
sw $31, -4($30)
sw $5, -8($30)
sw $6, -12($30)
sw $7, -16($30)
sw $13, -20($30)
sw $14, -24($30)
sw $9, -28($30)
sw $10, -32($30)
sw $16, -36($30)
sw $23, -40($30)
lis $31
.word 40
sub $30, $30, $31

add $5, $0, $0
add $6, $0, $0
add $7, $0, $0
add $12, $0, $0
add $13, $0, $0
add $14, $0, $0
add $9, $0, $0

lis $23
.word 3

dfsBegin:

mult $4, $10
mflo $16
add $16, $16, $1

lw $16, 0($16) ; get value at index

; check if index is divisible by 3
div $10, $23
mfhi $13 ; stores remainder

bne $13, $0, indexProducesIndex

indexProducesValue:
add $13, $0, $11 ; sets $13 to 1
beq $0, $0, cont

indexProducesIndex:
add $13, $0, $0 ; sets $13 to 0

cont:
bne $16, $21, dfsGeneric
bne $13, $0, dfsGeneric ; index must produce another index

; base case
add $12, $0, $0 ; return 0
beq $0, $0, dfsEpilogue

dfsGeneric:

lis $7
.word dfs

bne $13, $0, cont2
; index produces an index
add $10, $16, $0

cont2:
add $10, $10, $11 ; change index to left index by incrementing

jalr $7
add $5, $0, $12 ; copy over result to maxLeft
add $10, $10, $11 ; change index to right index by incrementing

jalr $7
add $6, $0, $12 ; copy over result to maxRight

slt $9, $5, $6 ; $9 = (left < right) 

bne $9, $11, lBigger

; here rBigger
add $12, $6, $0
add $12, $12, $11
beq $0, $0, dfsEpilogue

lBigger:
add $12, $5, $0
add $12, $12, $11
beq $0, $0, dfsEpilogue

dfsEpilogue:
lis $31
.word 40
add $30, $30, $31
lw $31, -4($30)
lw $5, -8($30)
lw $6, -12($30)
lw $7, -16($30)
lw $13, -20($30)
lw $14, -24($30)
lw $9, -28($30)
lw $10, -32($30)
lw $16, -36($30)
lw $23, -40($30)

jr $31

