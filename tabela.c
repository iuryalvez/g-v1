/*
 * tabela.c — Implementação da Tabela de Símbolos
 *
 * Implementa as operações de uma pilha de tabelas de símbolos.
 * Cada tabela na pilha representa o escopo de um bloco da linguagem G-V1.
 *
 * Estrutura visual da pilha durante a análise de um programa aninhado:
 *
 *   topo →  [ y : car  ]   ← escopo do bloco interno
 *           [ x : int  ]   ← escopo do bloco externo
 *   base →  (NULL)
 *
 * A busca por um símbolo começa no topo e desce até encontrar ou chegar
 * ao NULL (variável não declarada).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabela.h"

/*
 * iniciarPilha — aloca e retorna uma pilha de tabelas vazia.
 *
 * O topo começa como NULL porque ainda não há nenhum escopo aberto.
 */
PilhaTabela *iniciarPilha() {
    PilhaTabela *pilha = (PilhaTabela *)malloc(sizeof(PilhaTabela));
    if (!pilha) {
        fprintf(stderr, "Erro fatal: sem memória para a pilha de tabelas\n");
        exit(1);
    }
    pilha->topo = NULL;
    return pilha;
}

/*
 * empilharEscopo — abre um novo escopo, colocando uma tabela vazia no topo.
 *
 * O campo 'anterior' da nova tabela aponta para o escopo que estava no topo
 * antes — é assim que a pilha fica encadeada.
 */
void empilharEscopo(PilhaTabela *pilha) {
    Tabela *nova = (Tabela *)malloc(sizeof(Tabela));
    if (!nova) {
        fprintf(stderr, "Erro fatal: sem memória para novo escopo\n");
        exit(1);
    }
    nova->entradas = NULL;       /* começa sem nenhuma variável         */
    nova->anterior = pilha->topo; /* encadeia com o escopo anterior     */
    pilha->topo = nova;           /* a nova tabela passa a ser o topo   */
}

/*
 * inserirSimbolo — declara uma variável no escopo atual.
 *
 * Antes de inserir, verifica se o nome já foi declarado NESTE mesmo escopo.
 * (Redeclaração no mesmo escopo é um erro semântico em G-V1.)
 *
 * Retorna 0 em caso de sucesso, 1 se o nome já existe no escopo atual.
 */
int inserirSimbolo(PilhaTabela *pilha, const char *nome, TipoVar tipo, int linha) {
    if (!pilha->topo) {
        fprintf(stderr, "Erro interno: tentativa de inserir símbolo sem escopo aberto\n");
        exit(1);
    }

    /* verifica redeclaração no mesmo escopo */
    EntradaTabela *atual = pilha->topo->entradas;
    while (atual) {
        if (strcmp(atual->nome, nome) == 0) {
            return 1; /* nome já existe neste escopo → erro semântico */
        }
        atual = atual->prox;
    }

    /* cria a nova entrada e insere no início da lista (mais rápido) */
    EntradaTabela *nova = (EntradaTabela *)malloc(sizeof(EntradaTabela));
    if (!nova) {
        fprintf(stderr, "Erro fatal: sem memória para entrada de tabela\n");
        exit(1);
    }
    nova->nome  = strdup(nome); /* copia o nome para memória própria   */
    nova->tipo  = tipo;
    nova->linha = linha;
    nova->prox  = pilha->topo->entradas; /* encadeia com a lista atual */
    pilha->topo->entradas = nova;         /* nova entrada vira o início */

    return 0; /* sucesso */
}

/*
 * buscarSimbolo — procura um nome percorrendo a pilha do topo à base.
 *
 * Começa no escopo mais interno (topo). Se não encontrar ali, sobe para
 * o escopo anterior, e assim por diante. Isso garante que variáveis do
 * escopo interno "sombreiam" variáveis de mesmo nome do escopo externo.
 *
 * Retorna ponteiro para a entrada encontrada, ou NULL se não existir.
 */
EntradaTabela *buscarSimbolo(PilhaTabela *pilha, const char *nome) {
    Tabela *escopo = pilha->topo;

    /* percorre cada tabela da pilha, do topo à base */
    while (escopo) {
        EntradaTabela *entrada = escopo->entradas;

        /* percorre as variáveis dentro deste escopo */
        while (entrada) {
            if (strcmp(entrada->nome, nome) == 0) {
                return entrada; /* encontrou! retorna a entrada */
            }
            entrada = entrada->prox;
        }

        escopo = escopo->anterior; /* sobe para o escopo externo */
    }

    return NULL; /* não encontrou em nenhum escopo */
}

/*
 * desempilharEscopo — fecha o escopo atual, removendo o topo da pilha.
 *
 * Libera todas as entradas (variáveis) do escopo que está sendo fechado
 * e depois libera a própria tabela. O topo passa a ser o escopo anterior.
 */
void desempilharEscopo(PilhaTabela *pilha) {
    if (!pilha->topo) {
        fprintf(stderr, "Erro interno: tentativa de desempilhar escopo vazio\n");
        exit(1);
    }

    Tabela *removida = pilha->topo;

    /* libera todas as entradas (variáveis) deste escopo */
    EntradaTabela *entrada = removida->entradas;
    while (entrada) {
        EntradaTabela *prox = entrada->prox;
        free(entrada->nome); /* libera a cópia do nome */
        free(entrada);
        entrada = prox;
    }

    pilha->topo = removida->anterior; /* o escopo anterior volta ao topo */
    free(removida);
}

/*
 * liberarPilha — libera toda a memória da pilha.
 *
 * Desempilha todos os escopos que ainda estiverem abertos e depois
 * libera a estrutura da pilha em si.
 */
void liberarPilha(PilhaTabela *pilha) {
    while (pilha->topo) {
        desempilharEscopo(pilha);
    }
    free(pilha);
}
