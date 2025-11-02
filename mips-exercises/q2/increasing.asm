sw $31, -4($30) ; push $31 on stack
lis $31
.word 4
sub $30, $30, $31

add $9, $0, $0 ; index of array
lis $11
.word 1
lis $4
.word 4
lis $26
.word 0xffff000c ; output value

add $21, $1, $0 ; copy over 1
add $22, $2, $0 ; copy over 2
sub $23, $22, $11 ; length - 1

loop: 

beq $9, $23, done ; while index < length - 1

add $10, $9, $11 ; index + 1
mult $4, $9 ; 4 * index
mflo $19
add $20, $19, $4 ; 4 * index + 4
add $19, $19, $21
add $20, $20, $21
lw $1, 0($19) ; a[index]
lw $2, 0($20) ; a[index + 1]

lis $3 ; stores address of compare
.word compare

jalr $3 ; call function, should return value in $3

beq $3, $0, failed ; because not increasing

add $9, $9, $11 ; increment index
beq $0, $0, loop ; jump back to start of loop

failed:
add $3, $0, $0
lis $31 ; pop $31 from stack
.word 4
add $30, $30, $31
lw $31, -4($30)
jr $31

done:
add $3, $11, $0 
lis $31 ; pop $31 from stack
.word 4
add $30, $30, $31
lw $31, -4($30)
jr $31

