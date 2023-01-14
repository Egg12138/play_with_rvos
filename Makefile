files = dep1 dep2
# LOVE FOR C COMPILATION
# implicit rules to follow:
CC = gcc # or other c compiler
CXX = g++ # or other cpp compiler
CFLAGS = -Wall -g -O # arguments for gcc compiling c prog
CXXFLAGS = -c -o


thing_right = $(wildcard %.o)   #  Wildcard: % or *
# % can be used as 'matching' OR 'replacing' 
# matched items are called the `stem`
# Patterns below


automatic: arg1 arg2 dep1 dep2
	echo "the first target: $@"
	@echo "all prerequisites newer then automatic: $?"
	echo "[collected all vars: $^]"
	echo $?

arg1:
	echo "This is var1"
arg2:
	echo "This is var2"

blan.c:
	echo "[compiled].c -> .o -> prog"
	$(CC) blah.c 
some_file: $(files) dependencies
	echo "after dependencies..., see these vars:>" $(files)
	touch some_file
	touch new_some_file  # depends on `other files`

dep1:
	touch pkg1
dep2:
	touch pkg2


dependencies:
	echo "dependencies run first"	
	touch dependencies

clean:
	rm -rf  ./*.o some_file pkg*

print: $(wildcard *.c)
	exa -a $?

