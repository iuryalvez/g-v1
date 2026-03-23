# Links object files to create the final executable
g-v1: g-v1.o lexico.o
	gcc g-v1.o lexico.o -o g-v1

# Compiles the Bison-generated C file into an object file
g-v1.o: g-v1.c
	gcc -c g-v1.c -o g-v1.o

# Generates C source and header files from the Bison grammar
g-v1.c: g-v1.y
	bison -d -o g-v1.c g-v1.y

# Compiles the Flex-generated C file into an object file
lexico.o: lexico.c
	gcc -c lexico.c -o lexico.o

# Generates C source file from the Flex scanner
lexico.c: g-v1.l
	flex -o lexico.c g-v1.l

# Removes all generated build artifacts
clean: 
	rm -f *.o g-v1 *.c *.h