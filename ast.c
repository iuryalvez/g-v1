/*
 * ast.c — Implementação da Árvore Sintática Abstrata (AST)
 *
 * Este arquivo contém as funções para criar, imprimir e liberar nós da AST.
 * A criação dos nós é feita durante a análise sintática (g-v1.y), nas ações
 * semânticas associadas a cada regra da gramática.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/*
 * criarNo — função interna que aloca e inicializa um nó.
 *
 * Todos os campos de filhos começam como NULL. O campo 'valor' é copiado
 * com strdup() para que o nó tenha sua própria cópia do texto léxico.
 */
static No *criarNo(TipoNo tipo, int linha, const char *valor) {
    No *no = (No *)malloc(sizeof(No));
    if (!no) {
        fprintf(stderr, "Erro fatal: sem memória para alocar nó da AST\n");
        exit(1);
    }
    no->tipo   = tipo;
    no->linha  = linha;
    no->valor  = valor ? strdup(valor) : NULL; /* copia o texto ou NULL */
    no->filho1 = NULL;
    no->filho2 = NULL;
    no->filho3 = NULL;
    return no;
}

/*
 * novoNo — cria um nó folha (sem filhos).
 * Usado para: IDENTIFICADOR, INTCONST, CARCONST, CADEIACARACTERES,
 *             TIPO_INT, TIPO_CAR, CMD_NOVALINHA, CMD_VAZIO, etc.
 */
No *novoNo(TipoNo tipo, int linha, const char *valor) {
    return criarNo(tipo, linha, valor);
}

/*
 * novoNo1 — cria um nó com 1 filho.
 * Usado para: negação unária (-expr, !expr), CMD_EXPR, CMD_LEIA,
 *             CMD_ESCREVA_EXPR, AST_PROGRAMA, etc.
 */
No *novoNo1(TipoNo tipo, int linha, No *filho1) {
    No *no = criarNo(tipo, linha, NULL);
    no->filho1 = filho1;
    return no;
}

/*
 * novoNo2 — cria um nó com 2 filhos.
 * Usado para: operadores binários (soma, sub, atrib, ou, e, etc.),
 *             BLOCO, LISTA_DECL_VAR, LISTA_COMANDO, CMD_SE, CMD_ENQUANTO.
 */
No *novoNo2(TipoNo tipo, int linha, No *filho1, No *filho2) {
    No *no = criarNo(tipo, linha, NULL);
    no->filho1 = filho1;
    no->filho2 = filho2;
    return no;
}

/*
 * novoNo3 — cria um nó com 3 filhos.
 * Usado para: CMD_SE_SENAO (condição + then + else)
 *             e DECL_VAR (identificador + tipo + extras por vírgula).
 */
No *novoNo3(TipoNo tipo, int linha, No *filho1, No *filho2, No *filho3) {
    No *no = criarNo(tipo, linha, NULL);
    no->filho1 = filho1;
    no->filho2 = filho2;
    no->filho3 = filho3;
    return no;
}

/*
 * nomeTipo — retorna o nome textual de um TipoNo.
 * Usado apenas internamente por imprimirAST() para facilitar a leitura
 * da árvore durante depuração e apresentação.
 */
static const char *nomeTipo(TipoNo tipo) {
    switch (tipo) {
        case AST_PROGRAMA:          return "PROGRAMA";
        case AST_BLOCO:             return "BLOCO";
        case AST_LISTA_DECL_VAR:    return "LISTA_DECL_VAR";
        case AST_DECL_VAR:          return "DECL_VAR";
        case AST_TIPO_INT:          return "TIPO_INT";
        case AST_TIPO_CAR:          return "TIPO_CAR";
        case AST_LISTA_COMANDO:     return "LISTA_COMANDO";
        case AST_CMD_VAZIO:         return "CMD_VAZIO";
        case AST_CMD_EXPR:          return "CMD_EXPR";
        case AST_CMD_LEIA:          return "CMD_LEIA";
        case AST_CMD_ESCREVA_EXPR:  return "CMD_ESCREVA_EXPR";
        case AST_CMD_ESCREVA_STR:   return "CMD_ESCREVA_STR";
        case AST_CMD_NOVALINHA:     return "CMD_NOVALINHA";
        case AST_CMD_SE:            return "CMD_SE";
        case AST_CMD_SE_SENAO:      return "CMD_SE_SENAO";
        case AST_CMD_ENQUANTO:      return "CMD_ENQUANTO";
        case AST_ATRIB:             return "ATRIB";
        case AST_OU:                return "OU";
        case AST_E:                 return "E";
        case AST_IGUAL:             return "IGUAL";
        case AST_DIFERENTE:         return "DIFERENTE";
        case AST_MENOR:             return "MENOR";
        case AST_MAIOR:             return "MAIOR";
        case AST_MAIORIGUAL:        return "MAIORIGUAL";
        case AST_MENORIGUAL:        return "MENORIGUAL";
        case AST_SOMA:              return "SOMA";
        case AST_SUB:               return "SUB";
        case AST_MUL:               return "MUL";
        case AST_DIV:               return "DIV";
        case AST_NEG:               return "NEG";
        case AST_NAO:               return "NAO";
        case AST_IDENTIFICADOR:     return "IDENTIFICADOR";
        case AST_CARCONST:          return "CARCONST";
        case AST_INTCONST:          return "INTCONST";
        case AST_CADEIACARACTERES:  return "CADEIACARACTERES";
        default:                    return "DESCONHECIDO";
    }
}

/*
 * imprimirAST — imprime a árvore de forma indentada no terminal.
 *
 * Cada nível de profundidade adiciona 2 espaços de indentação.
 * Nós com valor léxico mostram o texto entre aspas.
 * A função percorre recursivamente: nó atual → filho1 → filho2 → filho3.
 */
void imprimirAST(No *no, int profundidade) {
    if (!no) return;

    /* imprime indentação proporcional à profundidade na árvore */
    for (int i = 0; i < profundidade; i++) printf("  ");

    printf("[%s]", nomeTipo(no->tipo));
    if (no->valor) printf(" \"%s\"", no->valor); /* mostra o lexema, se houver */
    printf(" (linha %d)\n", no->linha);

    /* percorre os filhos recursivamente */
    imprimirAST(no->filho1, profundidade + 1);
    imprimirAST(no->filho2, profundidade + 1);
    imprimirAST(no->filho3, profundidade + 1);
}

/*
 * liberarAST — libera toda a memória alocada pela AST.
 *
 * O percurso é pós-ordem: libera os filhos antes do nó pai,
 * evitando acesso a memória já liberada.
 */
void liberarAST(No *no) {
    if (!no) return;
    liberarAST(no->filho1);
    liberarAST(no->filho2);
    liberarAST(no->filho3);
    if (no->valor) free(no->valor); /* libera a cópia do lexema */
    free(no);
}
