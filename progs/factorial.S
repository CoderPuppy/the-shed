const 5
call1 2
call2
halt
bzero 0 15 //if n is 0 go to zero
alloc 1
sts 0 0 // store n on the stack
const 1
sub 1 0 // subtract 1 from n
call1 -6 // call factorial with n-1
call2
lds 0 // grab n from the stack
nop
mul 0 1 // multiply n and the result of calling factorial
nop
nop
nop
addi 1 0 // grab the lower half of the mult to return
ret1
ret2
const 1
ret1
ret2