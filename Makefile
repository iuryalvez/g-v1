# Makefile — Compilador G-V1
#
# Para compilar tudo de uma vez, basta rodar: make
# Para limpar os arquivos gerados:            make clean

CC     = gcc
CFLAGS = -Wall -g

# Linka todos os objetos para gerar o executável 'g-v2'
g-v2: g-v2.o lexico.o ast.o tabela.o semantico.o gerador.o
	$(CC) g-v2.o lexico.o ast.o tabela.o semantico.o gerador.o -o g-v2

g-v2.o: g-v2.c ast.h tabela.h gerador.h
	$(CC) $(CFLAGS) -c g-v2.c -o g-v2.o

g-v2.c g-v2.h: g-v2.y
	bison -d -o g-v2.c g-v2.y

lexico.o: lexico.c g-v2.h ast.h
	$(CC) $(CFLAGS) -c lexico.c -o lexico.o

lexico.c: g-v2.l g-v2.h
	flex -o lexico.c g-v2.l

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c -o ast.o

tabela.o: tabela.c tabela.h
	$(CC) $(CFLAGS) -c tabela.c -o tabela.o

semantico.o: semantico.c ast.h tabela.h
	$(CC) $(CFLAGS) -c semantico.c -o semantico.o

gerador.o: gerador.c gerador.h ast.h tabela.h
	$(CC) $(CFLAGS) -c gerador.c -o gerador.o

clean:
	rm -f *.o g-v2 g-v2.c g-v2.h lexico.c *.s

# === ROTINA DE TESTES ===
test: g-v2
	@echo "\n================================================="
	@echo "        EXECUTANDO TESTES CORRETOS               "
	@echo "================================================="
	@for file in Testes_G-V2/Corretos/*.txt; do \
		echo "\n[TESTANDO] $$file"; \
		./g-v2 "$$file" || true; \
	done
	@echo "\n================================================="
	@echo "      EXECUTANDO TESTES DE ERRO SINTATICO        "
	@echo "================================================="
	@for file in Testes_G-V2/ErroSintatico/*.txt; do \
		echo "\n[TESTANDO] $$file"; \
		./g-v2 "$$file" || true; \
	done
	@echo "\n================================================="
	@echo "      EXECUTANDO TESTES DE ERRO SEMANTICO        "
	@echo "================================================="
	@for file in Testes_G-V2/ErroSemantico/*.txt; do \
		echo "\n[TESTANDO] $$file"; \
		./g-v2 "$$file" || true; \
	done
	@echo "\n================================================="
	@echo "              BATERIA FINALIZADA                 "
	@echo "=================================================\n"
