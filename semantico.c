#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tabela.h"

// Error reporting utility
void erroSemantico(const char *msg, int linha) {
    printf("ERRO SEMANTICO: %s - linha: %d\n", msg, linha);
    exit(1);
}

// Main recursive function for semantic analysis
TipoVar analisarSemantica(No *no, PilhaTabela *pilha) {
    if (!no) return -1; // -1 indicates no type/void

    switch (no->tipo) {
        case AST_PROGRAMA:
            analisarSemantica(no->filho1, pilha);
            break;

        case AST_BLOCO:
            empilharEscopo(pilha);
            analisarSemantica(no->filho1, pilha); // VarSection (Declarations)
            analisarSemantica(no->filho2, pilha); // ListaComando
            desempilharEscopo(pilha);
            break;

        case AST_LISTA_DECL_VAR:
        case AST_LISTA_COMANDO:
            analisarSemantica(no->filho1, pilha);
            analisarSemantica(no->filho2, pilha);
            break;

        case AST_DECL_VAR: {
            // Retrieves type from child2 and identifier from child1
            TipoVar tipo = (no->filho2->tipo == AST_TIPO_INT) ? TIPO_INT : TIPO_CAR;
            char *nome = no->filho1->valor;

            if (inserirSimbolo(pilha, nome, tipo, no->linha) != 0) {
                erroSemantico("Variavel redeclarada no mesmo escopo", no->linha);
            }

            // Handles extra variables in the same line (e.g., x, y : int;)
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

        case AST_IDENTIFICADOR: {
            EntradaTabela *sym = buscarSimbolo(pilha, no->valor);
            if (!sym) {
                erroSemantico("Variavel nao declarada no escopo atual", no->linha);
            }
            return sym->tipo;
        }

        case AST_INTCONST:
            return TIPO_INT;

        case AST_CARCONST:
            return TIPO_CAR;

        case AST_ATRIB: {
            TipoVar tipoEsq = analisarSemantica(no->filho1, pilha); // Left-hand side
            TipoVar tipoDir = analisarSemantica(no->filho2, pilha); // Right-hand side

            if (tipoEsq != tipoDir) {
                erroSemantico("Atribuicao entre tipos incompativeis", no->linha);
            }
            return tipoEsq;
        }

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

        case AST_NEG: 
        case AST_NAO: {
            TipoVar tipo = analisarSemantica(no->filho1, pilha);
            // O operador unário negativo (-) e a negação lógica (!) exigem inteiro
            if (tipo != TIPO_INT) {
                erroSemantico("Operadores unarios ('-', '!') exigem operando do tipo int", no->linha);
            }
            return TIPO_INT;
        }

        case AST_OU:
        case AST_E: {
            TipoVar tipoEsq = analisarSemantica(no->filho1, pilha);
            TipoVar tipoDir = analisarSemantica(no->filho2, pilha);

            // Operadores lógicos exigem operandos inteiros (que funcionam como booleanos na G-V1)
            if (tipoEsq != TIPO_INT || tipoDir != TIPO_INT) {
                erroSemantico("Operadores logicos ('&', '|') exigem operandos do tipo int", no->linha);
            }
            return TIPO_INT;
        }

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
            return TIPO_INT; // Relational operations yield integer results (boolean logic)
        }

        case AST_CMD_EXPR:
        case AST_CMD_ESCREVA_EXPR:
            analisarSemantica(no->filho1, pilha);
            break;

        case AST_CMD_LEIA:
            // Checks if variable exists
            analisarSemantica(no->filho1, pilha);
            break;

        case AST_CMD_SE:
        case AST_CMD_ENQUANTO: {
            TipoVar tipoCond = analisarSemantica(no->filho1, pilha);
            if (tipoCond != TIPO_INT) {
                erroSemantico("A condicao deve resultar em tipo int", no->linha);
            }
            analisarSemantica(no->filho2, pilha); // Executes inner command/block
            break;
        }

        case AST_CMD_SE_SENAO: {
            TipoVar tipoCond = analisarSemantica(no->filho1, pilha);
            if (tipoCond != TIPO_INT) {
                erroSemantico("A condicao deve resultar em tipo int", no->linha);
            }
            analisarSemantica(no->filho2, pilha);
            analisarSemantica(no->filho3, pilha);
            break;
        }

        case AST_CMD_NOVALINHA:
        case AST_CMD_VAZIO:
        case AST_CMD_ESCREVA_STR:
            // Comandos estruturais simples que não afetam a tipagem e não possuem filhos a serem analisados.
            return -1;

        default:
            // Default traverse for unaffected nodes
            analisarSemantica(no->filho1, pilha);
            analisarSemantica(no->filho2, pilha);
            analisarSemantica(no->filho3, pilha);
            break;
    }
    return -1;
}