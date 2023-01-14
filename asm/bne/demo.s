
.text
.global _start


_start:
	
	li x5, 0; # int i = 0
	li x6, 5; # const 5, to b cmp	
loop:
	addi x5, x5, 1
	bne x5, x6, loop
stop:
	j stop
