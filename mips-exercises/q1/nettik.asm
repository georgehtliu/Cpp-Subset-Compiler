lis $5
.word 0xffff0004 ; input value
lis $6
.word 0xffffffff ; eof value
lis $8
.word 0x00000001 ; bytes value
add $3, $0, $0
lis $11
.word 1

printReverse:
; change $7 (input), $10 (for storing the function label)

; prologue
sw $7, -4($30)
sw $10, -8($30)
sw $31, -12($30)
lis $14
.word 12
sub $30, $30, $14

; what to do
lw $7, 0($5) ; take input in $7

beq $6, $7, done; if input is eof
lis $10
.word printReverse
jalr $10
sw $7, 0($4) ; output into terminal
add $3, $3, $8 ; increment

done:
; clean up by popping stack
lis $14
.word 12
add $30, $30, $14
lw $7, -4($30)
lw $10, -8($30)
lw $31, -12($30)
jr $31
 