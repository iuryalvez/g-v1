/*
 * semantico.c — Analisador Semântico
 *
 * Esta fase percorre a AST construída pelo analisador sintático e verifica
 * se o programa faz sentido segundo as regras da linguagem G-V1.
 *
 * O percurso é feito de forma recursiva, começando pela raiz da AST e
 * descendo em direção às folhas, sempre da esquerda para a direita.
 * Isso garante que as declarações de variáveis sejam processadas antes
 * dos comandos que as utilizam.
 *
 * Durante o percurso, a pilha de tabelas de símbolos (tabela.h) é usada
 * para controlar quais variáveis existem em cada escopo:
 *   - Ao entrar em um bloco: empilha um novo escopo
 *   - Ao encontrar uma declaração: insere a variável no escopo atual
 *   - Ao usar uma variável: busca na pilha (do topo à base)
 *   - Ao sair de um bloco: desempilha o escopo
 *
 * Erros detectados:
 *   - Variável usada sem ter sido declarada
 *   - Variável declarada duas vezes no mesmo escopo
 *   - Operação entre tipos incompatíveis (int com car)
 *   - Atribuição de tipo errado
 *   - Operadores aritméticos/lógicos aplicados a tipo car
 *   - Condição de 'se'/'enquanto' com tipo car
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tabela.h"

/*
 * erroSemantico — reporta um erro semântico e encerra a compilação.
 *
 * O formato da mensagem segue o padrão exigido pelo trabalho:
 * "ERRO SEMANTICO: <mensagem> - linha: <número>"
 *
 * A compilação é encerrada imediatamente com exit(1) porque não faz
 * sentido continuar após um erro semântico — o programa está incorreto.
 */
void erroSemantico(const char *msg, int linha) {
    printf("ERRO SEMANTICO: %s - linha: %d\n", msg, linha);
    exit(1);
}

/*
 * analisarSemantica — função principal recursiva da análise semântica.
 *
 * Recebe um nó da AST e a pilha de escopos atual.
 * Retorna o TipoVar do resultado da expressão (TIPO_INT ou TIPO_CAR),
 * ou -1 para nós que não são expressões (comandos, declarações, etc.).
 *
 * A função é chamada recursivamente para cada filho do nó atual,
 * simulando um percurso em profundidade na árvore.
 */
TipoVar analisarSemantica(No *no, PilhaTabela *pilha) {
    if (!no) return -1; /* nó vazio: sem tipo */

    switch (no->tipo) {

        /*
         * AST_PROGRAMA — raiz da árvore.
         * Apenas desce para o bloco principal.
         */
        case AST_PROGRAMA:
            analisarSemantica(no->filho1, pilha);
            break;

        /*
         * AST_BLOCO — bloco com declarações e comandos.
         *
         * Um bloco representa um novo escopo. O percurso é:
         *   1. Empilha um novo escopo vazio
         *   2. Analisa as declarações (filho1) → insere variáveis no escopo
         *   3. Analisa os comandos (filho2) → usa as variáveis inseridas
         *   4. Desempilha o escopo ao sair do bloco
         */
        case AST_BLOCO:
            empilharEscopo(pilha);
            analisarSemantica(no->filho1, pilha); /* declarações */
            analisarSemantica(no->filho2, pilha); /* comandos    */
            desempilharEscopo(pilha);
            break;

        /*
         * AST_LISTA_DECL_VAR / AST_LISTA_COMANDO — listas encadeadas.
         * Processa o primeiro elemento (filho1) e depois o restante (filho2).
         */
        case AST_LISTA_DECL_VAR:
        case AST_LISTA_COMANDO:
            analisarSemantica(no->filho1, pilha);
            analisarSemantica(no->filho2, pilha);
            break;

        /*
         * AST_DECL_VAR — declaração de uma variável.
         *
         * Obtém o nome (filho1) e o tipo (filho2) e tenta inserir na tabela.
         * Se o nome já existe no escopo atual → erro de redeclaração.
         *
         * Também processa variáveis extras declaradas na mesma linha com
         * vírgula (ex: "x, y, z : int"), que ficam encadeadas em filho3.
         */
        case AST_DECL_VAR: {
            TipoVar tipo = (no->filho2->tipo == AST_TIPO_INT) ? TIPO_INT : TIPO_CAR;
            char *nome = no->filho1->valor;

            if (inserirSimbolo(pilha, nome, tipo, no->linha) != 0) {
                erroSemantico("Variavel redeclarada no mesmo escopo", no->linha);
            }

            /* processa variáveis extras separadas por vírgula */
            No *extra = no->filho3;
            while (extra) {
                char *nomeExtra = extra->filho1->valor;
                if (inserirSimbolo(pilha, nomeExtra, tipo, extra->linha) != 0) {
                    erroSemantico("Variavel redeclarada no mesmo escopo", extra->linha);
                }
                extra = extra->filho2;
            }
            break;
        }

        /*
         * AST_IDENTIFICADOR — uso de uma variável em uma expressão.
         *
         * Busca o nome na pilha de escopos (do mais interno ao mais externo).
         * Se não encontrar → erro de variável não declarada.
         * Se encontrar → retorna o tipo da variável para checagem de tipos.
         */
        case AST_IDENTIFICADOR: {
            EntradaTabela *sym = buscarSimbolo(pilha, no->valor);
            if (!sym) {
                erroSemantico("Variavel nao declarada no escopo atual", no->linha);
            }
            return sym->tipo;
        }

        /* Constantes têm tipo fixo */
        case AST_INTCONST:
            return TIPO_INT;

        case AST_CARCONST:
            return TIPO_CAR;

        /*
         * AST_ATRIB — atribuição: id = expr
         *
         * Verifica se o tipo do lado esquerdo (variável) é igual ao tipo
         * do lado direito (expressão). Tipos diferentes → erro.
         */
        case AST_ATRIB: {
            TipoVar tipoEsq = analisarSemantica(no->filho1, pilha); /* variável */
            TipoVar tipoDir = analisarSemantica(no->filho2, pilha); /* expressão */

            if (tipoEsq != tipoDir) {
                erroSemantico("Atribuicao entre tipos incompativeis", no->linha);
            }
            return tipoEsq;
        }

        /*
         * Operadores aritméticos: +, -, *, /
         *
         * Exigem que ambos os operandos sejam int. Resultado é int.
         * Usar car em aritmética → erro.
         */
        case AST_SOMA:
        case AST_SUB:
        case AST_MUL:
        case AST_DIV: {
            TipoVar tipoEsq = analisarSemantica(no->filho1, pilha);
            TipoVar tipoDir = analisarSemantica(no->filho2, pilha);

            if (tipoEsq != TIPO_INT || tipoDir != TIPO_INT) {
                erroSemantico("Operadores aritmeticos exigem operandos do tipo int", no->linha);
            }
            return TIPO_INT;
        }

        /*
         * Operadores unários: - (negação) e ! (negação lógica)
         *
         * Ambos exigem operando int. Resultado é int.
         */
        case AST_NEG:
        case AST_NAO: {
            TipoVar tipo = analisarSemantica(no->filho1, pilha);
            if (tipo != TIPO_INT) {
                erroSemantico("Operadores unarios ('-', '!') exigem operando do tipo int", no->linha);
            }
            return TIPO_INT;
        }

        /*
         * Operadores lógicos: & (E) e || (OU)
         *
         * Na G-V1 não existe tipo booleano explícito. Operadores lógicos
         * trabalham com int (0 = falso, diferente de 0 = verdadeiro).
         * Usar car → erro.
         */
        case AST_OU:
        case AST_E: {
            TipoVar tipoEsq = analisarSemantica(no->filho1, pilha);
            TipoVar tipoDir = analisarSemantica(no->filho2, pilha);

            if (tipoEsq != TIPO_INT || tipoDir != TIPO_INT) {
                erroSemantico("Operadores logicos ('&', '||') exigem operandos do tipo int", no->linha);
            }
            return TIPO_INT;
        }

        /*
         * Operadores relacionais: ==, !=, <, >, <=, >=
         *
         * Exigem que os dois operandos sejam do MESMO tipo (ambos int ou
         * ambos car). Comparar int com car → erro.
         * O resultado é sempre int (0 ou 1), pois G-V1 não tem tipo booleano.
         */
        case AST_IGUAL:
        case AST_DIFERENTE:
        case AST_MENOR:
        case AST_MAIOR:
        case AST_MENORIGUAL:
        case AST_MAIORIGUAL: {
            TipoVar tipoEsq = analisarSemantica(no->filho1, pilha);
            TipoVar tipoDir = analisarSemantica(no->filho2, pilha);

            if (tipoEsq != tipoDir) {
                erroSemantico("Operadores relacionais exigem operandos de mesmo tipo", no->linha);
            }
            return TIPO_INT; /* resultado relacional é tratado como int */
        }

        /*
         * Comandos que contêm expressões: apenas desce para analisar.
         */
        case AST_CMD_EXPR:
        case AST_CMD_ESCREVA_EXPR:
            analisarSemantica(no->filho1, pilha);
            break;

        /*
         * AST_CMD_LEIA — leia id;
         * Verifica se a variável foi declarada (via análise do identificador).
         */
        case AST_CMD_LEIA:
            analisarSemantica(no->filho1, pilha);
            break;

        /*
         * AST_CMD_SE / AST_CMD_ENQUANTO — comandos condicionais e de laço.
         *
         * A condição (filho1) deve resultar em int — assim como em C,
         * 0 é falso e qualquer outro valor é verdadeiro.
         * Usar uma expressão car como condição → erro.
         */
        case AST_CMD_SE:
        case AST_CMD_ENQUANTO: {
            TipoVar tipoCond = analisarSemantica(no->filho1, pilha);
            if (tipoCond != TIPO_INT) {
                erroSemantico("A condicao deve resultar em tipo int", no->linha);
            }
            analisarSemantica(no->filho2, pilha); /* corpo do comando */
            break;
        }

        /*
         * AST_CMD_SE_SENAO — se com senão.
         * Mesma verificação de tipo na condição, mas analisa dois ramos.
         */
        case AST_CMD_SE_SENAO: {
            TipoVar tipoCond = analisarSemantica(no->filho1, pilha);
            if (tipoCond != TIPO_INT) {
                erroSemantico("A condicao deve resultar em tipo int", no->linha);
            }
            analisarSemantica(no->filho2, pilha); /* ramo then  */
            analisarSemantica(no->filho3, pilha); /* ramo senao */
            break;
        }

        /*
         * Comandos simples sem expressão a verificar.
         * novalinha, comando vazio e escreva string não envolvem tipos.
         */
        case AST_CMD_NOVALINHA:
        case AST_CMD_VAZIO:
        case AST_CMD_ESCREVA_STR:
            return -1;

        /*
         * Caso padrão: nós não tratados explicitamente.
         * Desce recursivamente para todos os filhos.
         */
        default:
            analisarSemantica(no->filho1, pilha);
            analisarSemantica(no->filho2, pilha);
            analisarSemantica(no->filho3, pilha);
            break;
    }
    return -1;
}
