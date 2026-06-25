/*
 * gerador.h — Geração de Código Assembly MIPS (G-V2)
 *
 * Última fase do compilador. Depois que a análise semântica confirma que
 * o programa está correto, esta fase percorre a AST mais uma vez e produz
 * instruções MIPS executáveis no SPIM/MARS.
 *
 * === O que mudou de G-V1 para G-V2 ===
 *
 *   - Variáveis GLOBAIS agora existem e vivem num bloco fixo de memória
 *     reservado em ".data", apontado pelo registrador $s1 (carregado uma
 *     única vez, no início do programa). Toda variável/vetor global é
 *     acessada como deslocamento a partir de $s1.
 *   - Variáveis e parâmetros LOCAIS continuam acessados via $fp, como em
 *     G-V1, só que agora $fp também serve de base para uma convenção de
 *     chamada de função completa (ver mais abaixo).
 *   - VETORES (locais, globais ou parâmetros) são representados de forma
 *     uniforme como um "ponteiro" de 4 bytes guardado no slot da
 *     variável — o mesmo esquema, seja o vetor local, global ou recebido
 *     por parâmetro:
 *       • vetor declarado (local ou global, com tamanho): o compilador
 *         reserva, ALÉM do slot do ponteiro, uma área separada do
 *         tamanho do vetor (a "área de dados" do vetor) e, na hora da
 *         declaração, calcula o endereço dessa área e guarda no slot.
 *       • vetor recebido como parâmetro (sem tamanho, passagem por
 *         referência): o slot já vem preenchido pelo chamador com o
 *         endereço do vetor original — nenhuma inicialização extra.
 *     Depois da declaração/passagem, TODO acesso a vetor (v[i], ou passar
 *     v como argumento) é igual: carrega o ponteiro do slot e soma
 *     índice × tamanho do elemento (4 bytes para int, 1 byte para car).
 *     Isso é o que torna a atribuição vetor=vetor (cópia de endereço, não
 *     de conteúdo, conforme o enunciado) trivial: é só copiar o ponteiro
 *     de um slot para o outro.
 *
 * === Convenção de chamada de função ===
 *
 * Cada função vira um label MIPS (`_func_NOME`). O chamador empilha os
 * argumentos do ÚLTIMO para o PRIMEIRO (assim o parâmetro de posição p
 * sempre fica em $fp + 8 + 4*(p-1), independente de quantos parâmetros a
 * função tem); a própria função (callee) é responsável por:
 *   1. salvar $ra e o $fp antigo na pilha e montar seu novo frame;
 *   2. reservar espaço para variáveis/vetores locais do bloco mais
 *      externo;
 *   3. ao final (`retorne` ou fim do corpo): desfazer tudo isso e também
 *      devolver o espaço dos argumentos empilhados pelo chamador
 *      ("callee-pop") — o chamador não precisa limpar nada depois do
 *      `jal`.
 *
 * Os registradores temporários $t0–$t9 NÃO são preservados através de uma
 * chamada de função (convenção padrão do MIPS). Por isso, sempre que o
 * gerador emite uma chamada, ele primeiro salva na pilha qualquer
 * temporário que o código que está "no meio" de uma expressão ainda
 * precise depois da chamada (ex: ao gerar "1 + f(2)", o valor "1" tem que
 * sobreviver à chamada de f) e os restaura depois do `jal`.
 *
 * === Registradores utilizados ===
 *
 *   $s1  — base fixa da área de variáveis globais (nunca muda de valor)
 *   $fp  — base do frame da função/bloco atual (variáveis e parâmetros
 *          locais são deslocamentos a partir dele)
 *   $sp  — topo da pilha de execução
 *   $ra  — endereço de retorno de uma chamada (jal/jr)
 *   $t0–$t9 — temporários de expressão (não sobrevivem a chamadas)
 *   $v0  — valor de retorno de função / número de syscall
 *   $a0  — argumento de syscall (valor a imprimir/ler, endereço de string)
 *
 * === Tamanhos ===
 *
 *   int            → 4 bytes (word)
 *   car            → 1 byte, mas o SLOT da variável ocupa 4 bytes
 *                    (alinhamento simples, como em G-V1)
 *   vetor de int   → slot de 4 bytes (ponteiro) + N*4 bytes de dados
 *   vetor de car   → slot de 4 bytes (ponteiro) + N*1 byte de dados
 */

#ifndef GERADOR_H
#define GERADOR_H

#include <stdio.h>
#include "ast.h"
#include "tabela.h"

/*
 * EntradaGerador — uma variável ou parâmetro conhecido pelo gerador.
 *
 * 'offset' é sempre o deslocamento do SLOT da variável (o valor em si,
 * para escalares; o ponteiro para os dados, no caso de vetores) — nunca
 * o deslocamento da área de dados do vetor, que só importa no instante
 * da declaração (ver gerarDeclaracoesLocais/Globais em gerador.c).
 *
 * 'global' indica a base usada para acessar o slot: 1 → deslocamento a
 * partir de $s1 (variável global); 0 → deslocamento a partir de $fp
 * (variável local ou parâmetro da função atual).
 */
typedef struct EntradaGerador {
    char *nome;
    TipoVar tipo;                 /* TIPO_INT, TIPO_CAR, TIPO_VETOR_INT/CAR */
    int offset;                   /* deslocamento do slot (ver acima)       */
    int global;                   /* 1 = via $s1, 0 = via $fp                */
    struct EntradaGerador *prox;
} EntradaGerador;

typedef struct TabelaGerador {
    EntradaGerador *entradas;
    struct TabelaGerador *anterior;
    int tamanho;                  /* bytes alocados neste escopo (p/ liberar) */
} TabelaGerador;

typedef struct {
    TabelaGerador *topo;
    int offsetAtual;              /* próximo deslocamento livre a partir de $fp */
} PilhaGerador;

/*
 * FuncaoGerador — registro mínimo de uma função, usado apenas para
 * descobrir o TIPO DE RETORNO quando uma chamada aparece dentro de
 * "escreva"/"leia" (para escolher a syscall certa: int vs car). Não
 * precisa de pilha de escopos — funções em G-V2 são todas globais.
 */
typedef struct FuncaoGerador {
    char *nome;
    TipoVar tipoRetorno;
    struct FuncaoGerador *prox;
} FuncaoGerador;

/*
 * gerarCodigo — ponto de entrada da geração de código.
 * Recebe a raiz da AST (já validada pela análise semântica) e o arquivo
 * de saída onde o assembly MIPS será escrito.
 */
void gerarCodigo(No *raiz, FILE *saida);

#endif /* GERADOR_H */
