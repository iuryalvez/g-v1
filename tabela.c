/*
 * tabela.c — Implementação da Tabela de Símbolos para G-V2
 *
 * Implementa as operações de uma pilha de tabelas de símbolos. Cada
 * tabela na pilha representa o escopo de um bloco da linguagem G-V2.
 * A base da pilha é sempre o escopo global (escopo 0), criado já em
 * iniciarPilha() — é lá que entram as variáveis globais e os nomes de
 * função.
 *
 * Estrutura visual da pilha durante a análise de um programa que
 * declara variáveis globais x, y, uma função f(a) e está, no momento,
 * dentro de um bloco aninhado no corpo de f:
 *
 *   topo →  [ c : car ]            ← escopo 2 (bloco aninhado em f)
 *           [ a : int (parâmetro), b : car ]  ← escopo 1 (corpo de f)
 *   base →  [ x : int, y : car, f ]            ← escopo 0 (global)
 *
 * A busca por um símbolo começa no topo e desce até encontrar ou
 * chegar à base sem encontrar (símbolo não declarado, ou não visível
 * no ponto do programa onde foi usado).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabela.h"

/* ------------------------------------------------------------------ */
/* Funções auxiliares internas (não fazem parte da interface pública) */
/* ------------------------------------------------------------------ */

/*
 * criarTabela — aloca uma tabela (escopo) vazia, já encadeada com o
 * escopo anterior e com a profundidade informada.
 */
static Tabela *criarTabela(int escopo, Tabela *anterior) {
    Tabela *nova = (Tabela *)malloc(sizeof(Tabela));
    if (!nova) {
        fprintf(stderr, "Erro fatal: sem memória para novo escopo\n");
        exit(1);
    }
    nova->entradas = NULL;
    nova->anterior = anterior;
    nova->escopo   = escopo;
    return nova;
}

/*
 * buscarSimboloEscopoAtual — procura um nome apenas na tabela do topo
 * da pilha (o escopo atual), sem descer para os escopos externos.
 *
 * Usada pelas funções de inserção para detectar redeclaração: um nome
 * só é "repetido" se já existir no MESMO escopo; em um escopo mais
 * interno é permitido sombrear (esconder) um nome do escopo externo.
 */
static EntradaTabela *buscarSimboloEscopoAtual(PilhaTabela *pilha,
                                                const char *nome) {
    EntradaTabela *atual = pilha->topo->entradas;
    while (atual) {
        if (strcmp(atual->nome, nome) == 0) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

/*
 * liberarEntradas — libera a lista encadeada de símbolos de um escopo.
 *
 * Para entradas de categoria CATEGORIA_FUNCAO, libera também a lista
 * de assinatura de parâmetros pendurada na entrada.
 */
static void liberarEntradas(EntradaTabela *entradas) {
    while (entradas) {
        EntradaTabela *prox = entradas->prox;

        if (entradas->categoria == CATEGORIA_FUNCAO) {
            ParametroAssinatura *p = entradas->assinatura;
            while (p) {
                ParametroAssinatura *proxParam = p->prox;
                free(p->nome);
                free(p);
                p = proxParam;
            }
        }

        free(entradas->nome);
        free(entradas);
        entradas = prox;
    }
}

/*
 * removerTopo — remove e libera a tabela do topo da pilha, sem
 * verificar se é o escopo global. É o trabalho "bruto" compartilhado
 * por desempilharEscopo() (que proíbe remover o escopo 0) e por
 * liberarPilha() (que precisa remover até o escopo 0 ao final).
 */
static void removerTopo(PilhaTabela *pilha) {
    Tabela *removida = pilha->topo;
    liberarEntradas(removida->entradas);
    pilha->topo = removida->anterior;
    free(removida);
}

/* ------------------------------------------------------------------ */
/* Interface pública                                                  */
/* ------------------------------------------------------------------ */

/*
 * iniciarPilha — aloca a pilha e já abre o escopo global (escopo 0).
 *
 * Diferente da versão de G-V1 (onde a pilha nascia vazia), em G-V2 a
 * pilha nunca fica sem nenhum escopo aberto: o escopo global existe
 * do início ao fim da compilação, pois variáveis globais e funções
 * precisam estar visíveis em qualquer ponto do programa.
 */
PilhaTabela *iniciarPilha() {
    PilhaTabela *pilha = (PilhaTabela *)malloc(sizeof(PilhaTabela));
    if (!pilha) {
        fprintf(stderr, "Erro fatal: sem memória para a pilha de tabelas\n");
        exit(1);
    }
    pilha->topo = criarTabela(0, NULL); /* escopo global */
    return pilha;
}

/*
 * empilharEscopo — abre um novo escopo, colocando uma tabela vazia no
 * topo. A profundidade do novo escopo é a do escopo atual mais um.
 */
void empilharEscopo(PilhaTabela *pilha) {
    int proximoEscopo = pilha->topo->escopo + 1;
    pilha->topo = criarTabela(proximoEscopo, pilha->topo);
}

/*
 * desempilharEscopo — fecha o escopo atual, removendo o topo da pilha.
 *
 * O escopo global (escopo 0) nunca deve ser removido por esta função:
 * ele só desaparece ao final da compilação, em liberarPilha(). Uma
 * tentativa de removê-lo aqui indicaria um bug no compilador (por
 * exemplo, um desempilharEscopo() chamado sem o empilharEscopo()
 * correspondente), então o programa é interrompido com uma mensagem
 * clara em vez de corromper a pilha silenciosamente.
 */
void desempilharEscopo(PilhaTabela *pilha) {
    if (!pilha->topo) {
        fprintf(stderr, "Erro interno: tentativa de desempilhar escopo vazio\n");
        exit(1);
    }
    if (pilha->topo->escopo == 0) {
        fprintf(stderr, "Erro interno: tentativa de remover o escopo global\n");
        exit(1);
    }
    removerTopo(pilha);
}

/*
 * buscarSimbolo — procura um nome percorrendo a pilha do topo à base.
 *
 * Começa no escopo mais interno (topo). Se não encontrar ali, sobe
 * para o escopo anterior, e assim por diante até o escopo global.
 * Isso garante que nomes do escopo interno "sombreiam" nomes de mesmo
 * lexema dos escopos externos.
 */
EntradaTabela *buscarSimbolo(PilhaTabela *pilha, const char *nome) {
    Tabela *escopo = pilha->topo;

    while (escopo) {
        EntradaTabela *entrada = escopo->entradas;
        while (entrada) {
            if (strcmp(entrada->nome, nome) == 0) {
                return entrada;
            }
            entrada = entrada->prox;
        }
        escopo = escopo->anterior;
    }

    return NULL; /* não encontrado em nenhum escopo visível */
}

/*
 * inserirVariavel — declara uma variável (global ou local) no escopo
 * atual.
 */
EntradaTabela *inserirVariavel(PilhaTabela *pilha, const char *nome,
                                TipoVar tipo, int posicao, int linha) {
    if (buscarSimboloEscopoAtual(pilha, nome)) {
        return NULL; /* já existe um símbolo com este nome neste escopo */
    }

    EntradaTabela *nova = (EntradaTabela *)malloc(sizeof(EntradaTabela));
    if (!nova) {
        fprintf(stderr, "Erro fatal: sem memória para entrada de tabela\n");
        exit(1);
    }
    nova->nome           = strdup(nome);
    nova->categoria      = CATEGORIA_VARIAVEL;
    nova->tipo           = tipo;
    nova->escopo         = pilha->topo->escopo;
    nova->posicao        = posicao;
    nova->linha          = linha;
    nova->numParametros  = 0;
    nova->assinatura     = NULL;
    nova->prox           = pilha->topo->entradas;
    pilha->topo->entradas = nova;

    return nova;
}

/*
 * inserirParametro — declara um parâmetro formal no escopo atual.
 *
 * A implementação é quase idêntica a inserirVariavel; a única
 * diferença real é a categoria guardada na entrada, usada depois pela
 * análise semântica (por exemplo, para impedir que se tente declarar
 * um vetor com o mesmo nome de um parâmetro escalar no mesmo escopo —
 * o que já é pego de graça pela checagem de redeclaração).
 */
EntradaTabela *inserirParametro(PilhaTabela *pilha, const char *nome,
                                 TipoVar tipo, int posicao, int linha) {
    if (buscarSimboloEscopoAtual(pilha, nome)) {
        return NULL;
    }

    EntradaTabela *nova = (EntradaTabela *)malloc(sizeof(EntradaTabela));
    if (!nova) {
        fprintf(stderr, "Erro fatal: sem memória para entrada de tabela\n");
        exit(1);
    }
    nova->nome           = strdup(nome);
    nova->categoria      = CATEGORIA_PARAMETRO;
    nova->tipo           = tipo;
    nova->escopo         = pilha->topo->escopo;
    nova->posicao        = posicao;
    nova->linha          = linha;
    nova->numParametros  = 0;
    nova->assinatura     = NULL;
    nova->prox           = pilha->topo->entradas;
    pilha->topo->entradas = nova;

    return nova;
}

/*
 * inserirFuncao — declara uma função no escopo atual.
 *
 * 'posicao' não se aplica a funções (não ocupam posição em uma lista
 * de variáveis), então é guardada como 0. A assinatura dos parâmetros
 * começa vazia e é preenchida depois, uma chamada por parâmetro, com
 * adicionarParametroAssinatura().
 */
EntradaTabela *inserirFuncao(PilhaTabela *pilha, const char *nome,
                              TipoVar tipoRetorno, int linha) {
    if (buscarSimboloEscopoAtual(pilha, nome)) {
        return NULL;
    }

    EntradaTabela *nova = (EntradaTabela *)malloc(sizeof(EntradaTabela));
    if (!nova) {
        fprintf(stderr, "Erro fatal: sem memória para entrada de tabela\n");
        exit(1);
    }
    nova->nome           = strdup(nome);
    nova->categoria      = CATEGORIA_FUNCAO;
    nova->tipo           = tipoRetorno;
    nova->escopo         = pilha->topo->escopo;
    nova->posicao        = 0;
    nova->linha          = linha;
    nova->numParametros  = 0;
    nova->assinatura     = NULL;
    nova->prox           = pilha->topo->entradas;
    pilha->topo->entradas = nova;

    return nova;
}

/*
 * adicionarParametroAssinatura — anexa um parâmetro formal ao final
 * da lista de assinatura de uma função.
 *
 * É essencial inserir ao FINAL da lista (e não no início, como fazemos
 * com as entradas de um escopo) porque a ordem dos parâmetros importa:
 * a análise semântica vai comparar essa lista, posição por posição,
 * com os argumentos de uma chamada à função.
 */
void adicionarParametroAssinatura(EntradaTabela *funcao, const char *nome,
                                   TipoVar tipo) {
    ParametroAssinatura *novo = (ParametroAssinatura *)malloc(sizeof(ParametroAssinatura));
    if (!novo) {
        fprintf(stderr, "Erro fatal: sem memória para assinatura de parâmetro\n");
        exit(1);
    }
    novo->nome = strdup(nome);
    novo->tipo = tipo;
    novo->prox = NULL;

    if (!funcao->assinatura) {
        funcao->assinatura = novo;
    } else {
        ParametroAssinatura *ultimo = funcao->assinatura;
        while (ultimo->prox) {
            ultimo = ultimo->prox;
        }
        ultimo->prox = novo;
    }

    funcao->numParametros++;
}

/*
 * liberarPilha — libera toda a memória da pilha, inclusive o escopo
 * global (o que desempilharEscopo() nunca faz).
 */
void liberarPilha(PilhaTabela *pilha) {
    while (pilha->topo) {
        removerTopo(pilha);
    }
    free(pilha);
}

/* --- nomes legíveis para depuração (usados só por imprimirPilha) --- */

static const char *tipoParaTexto(TipoVar tipo) {
    switch (tipo) {
        case TIPO_INT:        return "int";
        case TIPO_CAR:        return "car";
        case TIPO_VETOR_INT:  return "int[]";
        case TIPO_VETOR_CAR:  return "car[]";
    }
    return "?";
}

static const char *categoriaParaTexto(CategoriaSimbolo categoria) {
    switch (categoria) {
        case CATEGORIA_VARIAVEL:  return "variavel";
        case CATEGORIA_PARAMETRO: return "parametro";
        case CATEGORIA_FUNCAO:    return "funcao";
    }
    return "?";
}

/*
 * imprimirPilha — imprime, do topo à base, cada escopo e seus
 * símbolos. Serve apenas para depuração manual da tabela de símbolos.
 */
void imprimirPilha(PilhaTabela *pilha) {
    Tabela *escopo = pilha->topo;

    while (escopo) {
        printf("--- escopo %d ---\n", escopo->escopo);
        EntradaTabela *entrada = escopo->entradas;
        while (entrada) {
            printf("  %-15s %-10s %-7s pos=%-3d linha=%d\n",
                   entrada->nome,
                   categoriaParaTexto(entrada->categoria),
                   tipoParaTexto(entrada->tipo),
                   entrada->posicao,
                   entrada->linha);

            if (entrada->categoria == CATEGORIA_FUNCAO) {
                ParametroAssinatura *p = entrada->assinatura;
                printf("    assinatura (%d parametro(s)): ", entrada->numParametros);
                while (p) {
                    printf("%s:%s%s", p->nome, tipoParaTexto(p->tipo),
                           p->prox ? ", " : "");
                    p = p->prox;
                }
                printf("\n");
            }

            entrada = entrada->prox;
        }
        escopo = escopo->anterior;
    }
}
