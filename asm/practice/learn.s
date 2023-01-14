
# Beginning of the text section
	.text
	.global _start
	#.global _entryl

_start:
#_entry1:
	li t0, 1	# b
	li t1, 2	# c
	li t2, 3	# e
	# use ja

	# let valued var to be at the saved register
	add x8, t0, t1	# a = b + c 
	sub x9, x8, t2	# d = a - e


stop:
	j stop

	.end

	
