stirling:

prologue:
sw $31, -4($30)
sw $11, -8($30)
sw $20, -12($30)
sw $1, -16($30)
sw $2, -20($30)
sw $9, -24($30) 
lis $11
.word 1
lis $31
.word 24
add $20, $0, $0
sub $30, $30, $31
lis $9
.word stirling

; (0,0) check
bne $0, $1, izero 
bne $0, $2, zeroi ; here $1 is 0 at the start of the statement
add $3, $0, $11
beq $0, $0, epilogue

; (i,0) check
izero: ; we know $1 can't be zero so just check $2
bne $0, $2, generic
add $3, $0, $0
beq $0, $0, epilogue

; (0,i) check
zeroi: ; we just return
add $3, $0, $0
beq $0, $0, epilogue

; generic check
generic:
sub $20, $1, $11 ; $20 = n - 1
add $1, $0, $20 ; $1 = n - 1
jalr $9
mult $20, $3 ; returned here
mflo $20 ; $20 = f(n-1,k) * (n-1)
sub $2, $2, $11 ; $2 = k - 1
jalr $9
add $3, $3, $20 ; $20 = f(n-1,k) * (n-1) + f(n-1,k-1)
beq $0, $0, epilogue

epilogue:
lis $31
.word 24
add $30, $30, $31

lw $31, -4($30)
lw $11, -8($30)
lw $20, -12($30)
lw $1, -16($30)
lw $2, -20($30)
lw $9, -24($30) 

jr $31