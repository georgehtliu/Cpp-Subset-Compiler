lis $3 ; line feed
.word 0x0000000A
lis $4 ; minus sign
.word 0x0000002D
lis $5 ; input value
.word 0xffff0004
lis $6 ; output value
.word 0xffff000c
lis $21 ; constant 1
.word 1
lis $22 ; constant 2
.word 2
lis $23 ; constant 3
.word 3
lis $28 ; constant 10
.word 10
lis $29 ; constant 100
.word 100
lis $19 ; constant 48 (0's ascii)
.word 48
lis $18 ; constant -1
.word -1

add $25, $0, $0 ; hold unsigned decimal
add $26, $0, $0 ; hold boolean negative value
add $20, $0, $0 ; keeps track of length

read:
lw $7, 0($5) ; contains read character

beq $7, $3, doneRead ; done reading
beq $7, $4, negative ; read negative sign
bne $7, $4, continueRead ; skip negative sign update

negative:
add $26, $26, $21 ; increment negative boolean value
beq $0, $0, read ; go back to start of loop

continueRead:
add $20, $20, $21 ; increment length

beq $20, $21, readFirstDigit ; if length is 1
beq $20, $22, readSecDigit ; if length is 2
beq $20, $23, readThirdDigit ; if length is 3

readFirstDigit: 
add $10, $7, $0
sub $10, $10, $19 ; subtract the ascii of 0
beq $0, $0, read ; go back to start of loop

readSecDigit:
add $11, $7, $0
sub $11, $11, $19 ; subtract the ascii of 0
beq $0, $0, read

readThirdDigit:
add $12, $7, $0
sub $12, $12, $19 ; subtract the ascii of 0
beq $0, $0, read

doneRead:

beq $20, $21, process1 ; length is 1
beq $20, $22, process2 ; length is 2
beq $20, $23, process3 ; length is 3

process1: ; add ones digit
add $25, $0, $10
beq $0, $0, doneProcess

process2: ; add 10*tens + ones
multu $10, $28
mflo $10
add $25, $10, $11
beq $0, $0, doneProcess

process3: ; add 100*hundreds + 10*tens + ones
multu $10, $29
mflo $10
multu $11, $28
mflo $11
add $25, $10, $11
add $25, $25, $12
beq $0, $0, doneProcess

doneProcess:
beq $26, $0, output ; skip handleNeg

handleNeg:
mult $25, $18
mflo $25

output:
sw $25, 0($6)

exit:
jr $31 
