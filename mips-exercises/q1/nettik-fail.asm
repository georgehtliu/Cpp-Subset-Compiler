lis $4
.word 0xffff000c ; output value
lis $5
.word 0xffff0004 ; input value
lis $6
.word 0xffffffff ; eof value
lis $8
.word 0x00000001 ; bytes value
add $3, $0, $0
lis $14 ; stores the value 4
.word 4
lis $19; stores the RAM counter
.word -4

loop:
lw $7, 0($5) ; take input in $7

beq $6, $7, output ; if input is eof, go to output

; if input is not eof
sw $7, $19($30) ; store into RAM
sub $19, $19, $14 ; decrement counter by 4
beq $0, $0, loop ; jump back to loop

output:
add $19, $19, $14 ; add counter by 4 to adjust

loop2:
beq $0, $19, done ; if counter is 0, we are done
lw $7, $19($30) ; load word from memory
sw $7, 0($4) ; output into terminal
add $3, $3, $8 ; update number of bytes
add $19, $19, $14
beq $0, $0, loop2

done:
jr $31
 