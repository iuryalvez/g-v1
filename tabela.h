/*
 * tabela.h — Tabela de Símbolos (pilha de escopos)
 *
 * Imagine a tabela de símbolos como uma pilha de folhas de papel.
 * Cada vez que o programa entra em um bloco { }, uma folha nova é
 * colocada no topo da pilha. Nessa folha ficam anotadas as variáveis
 * declaradas naquele bloco. Quando o bloco termina, a folha do topo
 * é jogada fora — e as variáveis declaradas ali deixam de existir.
 *
 * Essa estrutura implementa o controle de escopo da linguagem G-V1,
 * que funciona de forma semelhante à linguagem C.
 *
 * Exemplo:
 *
 *   principal
 *   {
 *     { x : int; }        ← escopo 1: x existe aqui
 *     {
 *       { y : car; }      ← escopo 2: y existe aqui (x também, no escopo 1)
 *       {
 *         x = 5;          ← ok: x foi declarado no escopo 1 (ainda visível)
 *         y = 'a';        ← ok: y foi declarado no escopo 2
 *       }
 *     }
 *                         ← escopo 2 encerrado: y não existe mais
 *   }
 *                         ← escopo 1 encerrado: x não existe mais
 */

#ifndef TABELA_H
#define TABELA_H

/* Tipos de variável possíveis na linguagem G-V1 */
typedef enum {
    TIPO_INT,   /* variável do tipo int */
    TIPO_CAR    /* variável do tipo car (caractere) */
} TipoVar;

/*
 * EntradaTabela — representa uma variável declarada.
 *
 * Cada entrada guarda o nome da variável, seu tipo e um ponteiro
 * para a próxima entrada na mesma tabela (lista encadeada).
 */
typedef struct EntradaTabela {
    char *nome;                  /* nome (lexema) da variável              */
    TipoVar tipo;                /* tipo: TIPO_INT ou TIPO_CAR             */
    int linha;                   /* linha onde foi declarada (para erros)  */
    struct EntradaTabela *prox;  /* próxima variável na mesma tabela       */
} EntradaTabela;

/*
 * Tabela — representa uma "folha" da pilha: o escopo de um bloco.
 *
 * Cada tabela é uma lista encadeada de variáveis declaradas naquele
 * bloco, mais um ponteiro para o escopo anterior (o de baixo na pilha).
 */
typedef struct Tabela {
    EntradaTabela *entradas;  /* lista de variáveis deste escopo          */
    struct Tabela *anterior;  /* escopo anterior (embaixo na pilha)       */
} Tabela;

/*
 * PilhaTabela — a pilha em si.
 *
 * O campo 'topo' aponta para o escopo mais recente (o bloco mais interno
 * que está sendo analisado no momento).
 */
typedef struct {
    Tabela *topo;  /* escopo atual (topo da pilha)                        */
} PilhaTabela;

/* --- Operações sobre a pilha de tabelas --- */

/*
 * iniciarPilha — cria e retorna uma pilha vazia.
 * Deve ser chamada uma única vez antes de iniciar a análise semântica.
 */
PilhaTabela *iniciarPilha();

/*
 * empilharEscopo — cria um novo escopo e o coloca no topo da pilha.
 * Chamada ao entrar em um bloco que possui declarações de variáveis.
 */
void empilharEscopo(PilhaTabela *pilha);

/*
 * inserirSimbolo — adiciona uma variável ao escopo atual (topo da pilha).
 * Retorna 0 se inseriu com sucesso.
 * Retorna 1 se o nome já foi declarado neste mesmo escopo (erro semântico).
 */
int inserirSimbolo(PilhaTabela *pilha, const char *nome, TipoVar tipo, int linha);

/*
 * buscarSimbolo — procura um nome em todos os escopos, do topo à base.
 *
 * Essa busca respeita o sombreamento de variáveis: se um nome x existe
 * no escopo interno e no externo, o do escopo interno é retornado.
 *
 * Retorna um ponteiro para a entrada encontrada, ou NULL se não existir.
 */
EntradaTabela *buscarSimbolo(PilhaTabela *pilha, const char *nome);

/*
 * desempilharEscopo — remove o escopo do topo da pilha.
 * Chamada ao sair de um bloco, liberando todas as variáveis locais a ele.
 */
void desempilharEscopo(PilhaTabela *pilha);

/*
 * liberarPilha — libera toda a memória da pilha de tabelas.
 * Chamada ao final da compilação.
 */
void liberarPilha(PilhaTabela *pilha);

#endif /* TABELA_H */
