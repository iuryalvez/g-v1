# Makefile — Compilador G-V1
#
# Para compilar tudo de uma vez, basta rodar: make
# Para limpar os arquivos gerados:            make clean

CC     = gcc
CFLAGS = -Wall -g  # -Wall ativa todos os avisos; -g gera info de depuração

# Passo final: linka todos os objetos para gerar o executável 'g-v1'
# Link all objects including the new semantic analyzer (semantico.o)
g-v1: g-v1.o lexico.o ast.o tabela.o semantico.o
	$(CC) g-v1.o lexico.o ast.o tabela.o semantico.o -o g-v1

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

# Compila o analisador semântico
# Compile the semantic analyzer object
semantico.o: semantico.c ast.h tabela.h
	$(CC) $(CFLAGS) -c semantico.c -o semantico.o

# Remove todos os arquivos gerados
clean:
	rm -f *.o g-v1 g-v1.c g-v1.h lexico.c

	# === ROTINA DE TESTES ===
# Executa o compilador contra todos os arquivos nas pastas de teste.
# O '|| true' impede que o make pare ao encontrar os exit(1) dos erros propositais.
test: g-v1
	@echo "\n================================================="
	@echo "        EXECUTANDO TESTES CORRETOS               "
	@echo "================================================="
	@for file in Testes-G.v1/Corretos/*.g; do \
		echo "\n[TESTANDO] $$file"; \
		./g-v1 "$$file" || true; \
	done
	@echo "\n================================================="
	@echo "      EXECUTANDO TESTES DE ERRO SINTATICO        "
	@echo "================================================="
	@for file in Testes-G.v1/ErroSintatico/*.g; do \
		echo "\n[TESTANDO] $$file"; \
		./g-v1 "$$file" || true; \
	done
	@echo "\n================================================="
	@echo "      EXECUTANDO TESTES DE ERRO SEMANTICO        "
	@echo "================================================="
	@for file in Testes-G.v1/ErroSemantico/*.g; do \
		echo "\n[TESTANDO] $$file"; \
		./g-v1 "$$file" || true; \
	done
	@echo "\n================================================="
	@echo "              BATERIA FINALIZADA                 "
	@echo "=================================================\n"