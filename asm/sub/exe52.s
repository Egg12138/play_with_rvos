# Substract
# Format:
#	SUB RD, RS1, RS2
# Description:
#	The contents of RS2 is subtracted from the contents of RS1 and the result
#	is placed in RD.

	.text			# Define beginning of text section
	.global	_start		# Define entry _start

_start:
	li x6, 1		# b = 1
	li x7, 2		# c = 2
	li x8, 3		# e = 3
	add x5, x6, x7		# a = b + c
	sub x9, x5, x8		# d = a - e

stop:
	j stop			# Infinite loop to stop execution

	.end			# End of file
