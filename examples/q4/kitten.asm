lis $4
.word 0xffff000c ; output value
lis $5
.word 0xffff0004 ; input value
lis $6
.word 0xffffffff ; eof value
lis $8
.word 0x00000001 ; bytes value
add $3, $0, $0
loop:
lw $7, 0($5)
bne $6, $7, output
beq $6, $7, done
output:
sw $7, 0($4) ; output into terminal
add $3, $3, $8
beq $0, $0, loop
done:
jr $31
 
