/*
 * gerador.h — Geração de Código Assembly MIPS
 *
 * Esta é a última fase do compilador. Após a análise semântica confirmar
 * que o programa está correto, esta fase percorre a AST mais uma vez e
 * produz instruções em linguagem assembly MIPS.
 *
 * O assembly MIPS gerado pode ser simulado com ferramentas como o SPIM
 * ou o MARS (Mars MIPS Assembler and Runtime Simulator).
 *
 * === Estratégia de alocação de memória ===
 *
 * A linguagem G-V1 tem blocos aninhados com variáveis locais. Para
 * gerenciar isso, usamos a PILHA DE EXECUÇÃO (stack) do MIPS:
 *
 *   - Ao entrar em um bloco com variáveis, reservamos espaço na pilha
 *     subtraindo do registrador $sp (stack pointer).
 *   - Cada variável recebe um "offset" (deslocamento) em relação ao $sp
 *     do bloco atual, que fica registrado na tabela de símbolos.
 *   - Ao sair do bloco, restauramos o $sp, devolvendo o espaço.
 *
 * === Registradores utilizados ===
 *
 *   $sp  — stack pointer: aponta para o topo da pilha
 *   $fp  — frame pointer: base do frame atual (usamos para acessar vars)
 *   $t0-$t9 — registradores temporários: guardam resultados intermediários
 *   $v0  — usado para syscalls (I/O)
 *   $a0  — argumento para syscalls (valor a imprimir, etc.)
 *   $ra  — endereço de retorno (usado pelo 'jal')
 *
 * === Tamanhos ===
 *
 *   int  → 4 bytes (word)
 *   car  → 1 byte  (byte), mas alinhado em 4 bytes na pilha por simplicidade
 */

#ifndef GERADOR_H
#define GERADOR_H

#include <stdio.h>
#include "ast.h"
#include "tabela.h"

/*
 * EntradaGerador — estende a tabela de símbolos para guardar o offset
 * de cada variável na pilha durante a geração de código.
 *
 * Como a tabela de símbolos original (tabela.h) já foi usada na análise
 * semântica, criamos uma nova pilha própria para a geração de código,
 * que além do tipo também guarda a posição na memória (offset).
 */
typedef struct EntradaGerador {
    char *nome;                  /* nome da variável                        */
    TipoVar tipo;                /* tipo: TIPO_INT ou TIPO_CAR              */
    int offset;                  /* posição em bytes relativa ao $fp        */
    struct EntradaGerador *prox; /* próxima variável na mesma tabela        */
} EntradaGerador;

typedef struct TabelaGerador {
    EntradaGerador *entradas;    /* lista de variáveis deste escopo         */
    struct TabelaGerador *anterior; /* escopo anterior                      */
    int tamanho;                 /* total de bytes alocados neste escopo    */
} TabelaGerador;

typedef struct {
    TabelaGerador *topo;         /* escopo atual                            */
    int offsetAtual;             /* offset acumulado relativo ao $fp        */
} PilhaGerador;

/* --- Funções principais --- */

/*
 * gerarCodigo — ponto de entrada da geração de código.
 * Recebe a raiz da AST e o arquivo de saída onde o assembly será escrito.
 */
void gerarCodigo(No *raiz, FILE *saida);

#endif /* GERADOR_H */
