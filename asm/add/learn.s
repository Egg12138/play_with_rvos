	.text
	.global _start

_start: 

	li x5, 1
	li x6, 2
	li x7, 3
	li x8, 4
	add x9, x5, x6
	beq x9, x7, loop


loop:
	loop

stop:

	j stop
	.end


.beqc



