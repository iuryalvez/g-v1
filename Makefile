# Makefile — Compilador G-V1
#
# Para compilar tudo de uma vez, basta rodar: make
# Para limpar os arquivos gerados:            make clean

CC     = gcc
CFLAGS = -Wall -g  # -Wall ativa todos os avisos; -g gera info de depuração

# Passo final: linka todos os objetos para gerar o executável 'g-v1'
g-v1: g-v1.o lexico.o ast.o tabela.o
	$(CC) g-v1.o lexico.o ast.o tabela.o -o g-v1

# Compila o arquivo C gerado pelo Bison
g-v1.o: g-v1.c ast.h tabela.h
	$(CC) $(CFLAGS) -c g-v1.c -o g-v1.o

# Gera g-v1.c e g-v1.h a partir da gramática
g-v1.c g-v1.h: g-v1.y
	bison -d -o g-v1.c g-v1.y

# Compila o arquivo C gerado pelo Flex
lexico.o: lexico.c g-v1.h ast.h
	$(CC) $(CFLAGS) -c lexico.c -o lexico.o

# Gera lexico.c a partir das expressões regulares
lexico.c: g-v1.l g-v1.h
	flex -o lexico.c g-v1.l

# Compila a AST
ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c -o ast.o

# Compila a tabela de símbolos
tabela.o: tabela.c tabela.h
	$(CC) $(CFLAGS) -c tabela.c -o tabela.o

# Remove todos os arquivos gerados
clean:
	rm -f *.o g-v1 g-v1.c g-v1.h lexico.c
