beq $1, $0, notdivide
divu $2, $1
mfhi $4
beq $4, $0, found
bne $4, $0, notdivide
found:
lis $4
.word 0x00000001
add $3, $4, $0
sub $4, $4, $4
beq $0, $0, exit
notdivide:
add $3, $0, $0
exit:
jr $31
