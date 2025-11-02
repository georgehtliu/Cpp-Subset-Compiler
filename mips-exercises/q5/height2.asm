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
add $9, $0, $0
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
; uses height ($9), index of node ($10), returns in $12

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
lis $31
.word 36
sub $30, $30, $31

add $5, $0, $0
add $6, $0, $0
add $7, $0, $0
add $12, $0, $0
add $13, $0, $0
add $14, $0, $0

dfsBegin:

mult $4, $10
mflo $16
add $16, $16, $1

lw $16, 0($16) ; value at index

bne $16, $21, dfsGeneric


; base case:
add $12, $9, $0 ; return just the height
beq $0, $0, dfsEpilogue

dfsGeneric:

lis $7
.word dfs

add $9, $9, $11 ; increment height by 1
add $10, $10, $11 ; change index to left index by incrementing

jalr $7
.word -1

add $13, $12, $0 ; store returned value in $13
add $10, $10, $11 ; change index to right index by incrementing

jalr $7
slt $14, $12, $13 ; if right < left
beq $14, $0, dfsEpilogue; if right >= left
add $12, $13, $0 ; copy over left height

dfsEpilogue:

lis $31
.word 36
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

jr $31

