
; registers used
; $5 readWord label
; $12 printHex label
; $6 endMod
; $7 codeSize
; $9, $19 trash
; $8 i
; $4 4
; $10 alpha
; $20 store 0x10000
; $15 store 1000

lis $15
.word 0x01000000

lis $20
.word 0x10000

lis $4
.word 4

; store readWord and printHex
lis $5
.word readWord
lis $12
.word printHex

; read alpha
sw $31, -4($30)
sub $30, $30, $4
jalr $5
; pop 31
add $30, $30, $4
lw $31, -4($30)
add $10, $3, $0

; read first word
; push 31
sw $31, -4($30)
sub $30, $30, $4
jalr $5
; pop 31
add $30, $30, $4
lw $31, -4($30)

; store endMod
; push 31
sw $31, -4($30)
sub $30, $30, $4
jalr $5
; pop 31
add $30, $30, $4
lw $31, -4($30)
add $6, $3, $0

; store codeSize
; push 31
sw $31, -4($30)
sub $30, $30, $4
jalr $5
; pop 31
add $30, $30, $4
lw $31, -4($30)
add $7, $3, $0
lis $9
.word 12
sub $7, $7, $9

; FIRST LOOP

; initialize loop
add $8, $0, $0

loop: ; loop

; read
; push 31
sw $31, -4($30)
sub $30, $30, $4
jalr $5
; pop 31
add $30, $30, $4
lw $31, -4($30)


; store into memory
add $9, $0, $10
add $9, $8, $9 ; 9 holds alpha + i

sw $3, 0($9)

; print
add $1, $3, $0 ; store value of reg 3 in reg 1 as argument for printHex
; push 31
sw $31, -4($30)
sub $30, $30, $4
jalr $12
; pop 31
add $30, $30, $4
lw $31, -4($30)


add $8, $8, $4 ; increment by 4
; check bound
bne $8, $7, loop ; go back to loop


; relocation stage
lis $9
.word 12
add $8, $7, $9 ; i = codeSize + 12

while:
; read
sw $31, -4($30)
sub $30, $30, $4
jalr $5
add $30, $30, $4
lw $31, -4($30)

add $27, $3, $0

; $27 has format

lis $11
.word 1

lis $22
.word 0x05

beq $27, $11, regular
beq $27, $22, esd

esr:
lis $1
.word 0x000EBB0B
; print
sw $31, -4($30)
sub $30, $30, $4
jalr $12
add $30, $30, $4
lw $31, -4($30)
jr $31


esd:
; read
sw $31, -4($30)
sub $30, $30, $4
jalr $5
add $30, $30, $4
lw $31, -4($30)

; read
sw $31, -4($30)
sub $30, $30, $4
jalr $5
add $30, $30, $4
lw $31, -4($30)

add $28, $3, $0 ; 28 has length
add $18, $0, $0 ; 18 is counter

lengthLoop:

; read
sw $31, -4($30)
sub $30, $30, $4
jalr $5
add $30, $30, $4
lw $31, -4($30)

; increment
lis $11
.word 1
add $18, $18, $11
bne $18, $28, lengthLoop

; done loop
; total memory accessed = 3*4 + length*4
lis $9
.word 12
mult $28, $4
mflo $28
add $28, $28, $9

; increment lines counter
add $8, $8, $28
; $6 = endMod 
slt $9, $8, $6
bne $9, $0, while

beq $0, $0, exitLoop

regular:
; read
sw $31, -4($30)
sub $30, $30, $4
jalr $5
add $30, $30, $4
lw $31, -4($30)

; $3 has rel
lis $9
.word 12
sub $9, $10, $9 ; $9 = alpha - 12
add $3, $3, $9 ; $3 = rel + alpha - 12

; store
lw $19, 0($3) ; $19 = mem[rel + alpha - 12]
add $19, $19, $9 ; $19 = mem[rel + alpha - 12] + alpha - 12
sw $19, 0($3) ; mem[rel + alpha - 12] = mem[rel + alpha - 12] + alpha - 12

; increment and check bounds
lis $9
.word 8
lis $11
.word 1
add $8, $8, $9
; $6 = endMod 
slt $9, $8, $6
bne $9, $0, while

exitLoop:

; get address
add $9, $10, $0 ; $9 holds alpha

; execution stage

; push all used registers
sw $31, -4($30)
sub $30, $30, $4
sw $15, -4($30)
sub $30, $30, $4
sw $20, -4($30)
sub $30, $30, $4
sw $5, -4($30)
sub $30, $30, $4
sw $12, -4($30)
sub $30, $30, $4
sw $6, -4($30)
sub $30, $30, $4
sw $7, -4($30)
sub $30, $30, $4
sw $9, -4($30)
sub $30, $30, $4
sw $8, -4($30)
sub $30, $30, $4
sw $4, -4($30)
sub $30, $30, $4
sw $10, -4($30)
sub $30, $30, $4

; store 30
lis $15
.word 0x1000
sw $30, 0($15)

; increase 30
add $30, $0, $15
add $30, $30, $20
add $30, $30, $10

; execute instruction
jalr $9

; load 30 back
lis $15
.word 0x1000
lw $30, 0($15)

lis $4
.word 4

; pop used registers
add $30, $30, $4
lw $10, -4($30)
add $30, $30, $4
lw $4, -4($30)
add $30, $30, $4
lw $8, -4($30)
add $30, $30, $4
lw $9, -4($30)
add $30, $30, $4
lw $7, -4($30)
add $30, $30, $4
lw $6, -4($30)
add $30, $30, $4
lw $12, -4($30)
add $30, $30, $4
lw $5, -4($30)
add $30, $30, $4
lw $20, -4($30)
add $30, $30, $4
lw $15, -4($30)
add $30, $30, $4
lw $31, -4($30)


; SECOND LOOP

; initialize loop
add $8, $0, $0

loop2: ; loop

; get address
add $9, $10, $0 
add $9, $10, $8 ; $9 holds alpha + i
lw $1, 0($9)

; print

; push 31
sw $31, -4($30)
sub $30, $30, $4
jalr $12
; pop 31
add $30, $30, $4
lw $31, -4($30)

add $8, $8, $4 ; increment by 4
; check bound
bne $8, $7, loop2 ; go back to loop

; end 
jr $31




readWord:
sw $1,  -4($30)
sw $2,  -8($30)
sw $4, -12($30)
sw $5, -16($30)
sw $6, -20($30)
sw $7, -24($30)
sw $8, -28($30)
lis $8
.word 28
sub $30, $30, $8
lis $4
.word 0x01000000
lis $3
.word 0x00010000
lis $2
.word 0x00000100
lis $1
.word 0xffff0004
lw $8, 0($1)
lw $7, 0($1)
lw $6, 0($1)
lw $5, 0($1)
multu $8, $4
mflo $8
multu $7, $3
mflo $7
multu $6, $2
mflo $6
add $4, $8, $7
add $3, $6, $5
add $3, $4, $3
lis $8
.word 28
add $30, $30, $8
lw $1,  -4($30)
lw $2,  -8($30)
lw $4, -12($30)
lw $5, -16($30)
lw $6, -20($30)
lw $7, -24($30)
lw $8, -28($30)
jr $31

printHex:
sw $1,  -4($30)
sw $2,  -8($30)
sw $3, -12($30)
sw $4, -16($30)
sw $5, -20($30)
sw $6, -24($30)
sw $7, -28($30)
sw $8, -32($30)
lis $8
.word 32
sub $30, $30, $8
lis $2
.word 0x10000000
lis $3
.word 0x10
lis $4
.word 0x30
lis $6
.word 10
lis $7
.word 7  
lis $8
.word 0xffff000c
PHloop:
divu $1, $2
mflo $1
slt $5, $1, $6 
bne $5, $0, PHskip
        add $1, $1, $7
PHskip: add $1, $1, $4 
sw $1, 0($8)
mfhi $1
divu $2, $3
mflo $2
bne $2, $0, PHloop
sw $6, 0($8)
lis $8
.word 32
add $30, $30, $8
lw $1,  -4($30)
lw $2,  -8($30)
lw $3, -12($30)
lw $4, -16($30)
lw $5, -20($30)
lw $6, -24($30)
lw $7, -28($30)
lw $8, -32($30)
done:
jr $31