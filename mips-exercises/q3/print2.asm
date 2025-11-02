sw $31, -4($30)
lis $31
.word 4
sub $30, $30, $31
lis $10
.word print

lis $1
.word -241
jalr $10


lis $1
.word 57
jalr $10

lis $1
.word 404
jalr $10

lis $31
.word 4
add $30, $30, $31
lw $31, -4($30)
jr $31

print:
; prologue
sw $11, -4($30)
sw $29, -8($30)
sw $28, -12($30)
sw $27, -16($30)
sw $26, -20($30)
sw $25, -24($30)
sw $21, -28($30)
sw $4, -32($30)
sw $10, -36($30)
sw $12, -40($30)
sw $18, -44($30)
sw $11, -48($30)
sw $19, -52($30)

lis $11
.word 52
sub $30, $30, $11

lis $29
.word 0xffff000c ; output address

lis $28 ; constant 48 (0's ascii)
.word 48

lis $27 ; minus sign
.word 0x0000002D

lis $26 ; input address
.word 0xffff0004

lis $25 ; line feed
.word 0x0000000A

lis $21 ; constant 1
.word 1

lis $4 ; constant 4
.word 4

lis $10 ; constant 10
.word 10

lis $12 ; constant -1
.word -1

add $18, $0, $0 ; length

add $11, $1, $0 ; copy $1 to $11

slt $19, $1, $0 ; set negative flag

; place these on stack and $31

bne $19, $21, read ; if non negative, skip
mult $11, $12
mflo $11

read:

divu $11, $10
mfhi $11 ; $11 stores the remainder

sw $11, -4($30) ; store digit in memory
sub $30, $30, $4 ; decrement stack pointer by 4
add $18, $18, $21 ; increment length by 1

mflo $11 ; $11 stores the new quotient
beq $11, $0, doneRead
beq $0, $0, read

doneRead:

bne $19, $21, printFinal ; if not negative, continue
sw $27, 0($29) ; print out negative sign

printFinal:
beq $18, $0, epilogue

lw $11, 0($30) ; load number from address
add $11, $11, $28 ; get ascii 
sw $11, 0($29) ; print number
add $30, $30, $4 ; increment stack pointer by 4
sub $18, $18, $21 ; decrement length
beq $0, $0, printFinal

epilogue:
sw $25, 0($29) ; print line feed
lis $11
.word 52
add $30, $30, $11
lw $11, -4($30)
lw $29, -8($30)
lw $28, -12($30)
lw $27, -16($30)
lw $26, -20($30)
lw $25, -24($30)
lw $21, -28($30)
lw $4, -32($30)
lw $10, -36($30)
lw $12, -40($30)
lw $18, -44($30)
lw $11, -48($30)
lw $19, -52($30)

jr $31

