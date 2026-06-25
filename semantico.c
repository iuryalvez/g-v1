/*
 * semantico.c — Analisador Semântico (G-V2)
 *
 * Reescrita para a nova API da tabela de símbolos (tabela.h), que agora
 * tem escopo global dedicado, funções e vetores. As regras implementadas
 * seguem a Seção 6 do enunciado do Trabalho 2.
 *
 * === Estratégia geral ===
 *
 * 1. Variáveis globais (seção `global[...]`) são inseridas no escopo 0
 *    ANTES de qualquer outra coisa — ficam visíveis o resto da compilação.
 *
 * 2. Funções (seção `funcao[...]`) são processadas em DUAS PASSADAS:
 *      passada 1 — registra todos os cabeçalhos (nome, tipo de retorno,
 *                  assinatura dos parâmetros) no escopo global, sem olhar
 *                  para dentro dos corpos. Isso permite uma função chamar
 *                  outra declarada depois dela (inclusive recursão mútua),
 *                  sem precisar de protótipo.
 *      passada 2 — agora que todo cabeçalho já existe na tabela, analisa
 *                  o corpo de cada função (parâmetros + bloco mais externo
 *                  compartilhando o MESMO escopo, comandos, retorne).
 *
 * 3. O bloco `principal` é só mais um AST_BLOCO, processado pelo caminho
 *    genérico do switch (igual a qualquer bloco aninhado).
 *
 * === Detalhe da gramática que precisa de atenção (g-v2.y) ===
 *
 * Numa linha "x, y, z : int", o Bison só guarda o nó de Tipo (filho2) no
 * primeiro identificador da lista (a "cabeça"); os demais (encadeados via
 * filho3) ficam com filho2 = NULL. Por isso NUNCA lemos o tipo direto de
 * um nó encadeado — o tipo é descoberto uma vez, na cabeça da lista, e
 * repassado como parâmetro pelas funções auxiliares abaixo enquanto elas
 * percorrem a cadeia (filho3).
 *
 * === Estado "função atual" ===
 *
 * Como uma função não pode ser declarada dentro de outra em G-V2 (todas
 * ficam numa lista plana antes do `principal`), duas variáveis globais a
 * este arquivo bastam para checar `retorne` corretamente: o tipo de
 * retorno esperado e se estamos (ou não) dentro do corpo de alguma função
 * neste momento do percurso.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tabela.h"

#define MAX_ARGS 64

/* --- estado da "função atual", usado por AST_CMD_RETORNE --- */
static bool    dentroDeFuncao   = false;
static TipoVar tipoRetornoAtual = TIPO_INT;

/*
 * erroSemantico — reporta um erro semântico e encerra a compilação.
 * Mesmo formato usado em G-V1: "ERRO SEMANTICO: <msg> - linha: <n>"
 */
void erroSemantico(const char *msg, int linha) {
    printf("ERRO SEMANTICO: %s - linha: %d\n", msg, linha);
    exit(1);
}

TipoVar analisarSemantica(No *no, PilhaTabela *pilha);

/* ------------------------------------------------------------------ */
/* Auxiliares: declaração de variáveis (globais, locais, de bloco)    */
/* ------------------------------------------------------------------ */

/*
 * processarDeclVarChain — insere uma variável (ou vetor) e, se houver,
 * continua pela cadeia de nomes encadeados na mesma linha (filho3).
 *
 * 'tipoBase' é o tipo já descoberto na cabeça da lista (TIPO_INT ou
 * TIPO_CAR) — NÃO lemos decl->filho2 aqui, pois só a cabeça o carrega.
 */
static void processarDeclVarChain(No *decl, TipoVar tipoBase,
                                   PilhaTabela *pilha, int *posicao) {
    if (!decl) return;

    No *alvo = decl->filho1; /* AST_IDENTIFICADOR ou AST_VETOR_DECL */
    char *nome;
    TipoVar tipoFinal;

    if (alvo->tipo == AST_VETOR_DECL) {
        nome = alvo->filho1->valor; /* filho1 do VETOR_DECL = identificador */
        tipoFinal = (tipoBase == TIPO_INT) ? TIPO_VETOR_INT : TIPO_VETOR_CAR;
    } else {
        nome = alvo->valor;
        tipoFinal = tipoBase;
    }

    if (!inserirVariavel(pilha, nome, tipoFinal, *posicao, decl->linha)) {
        erroSemantico("Variavel redeclarada no mesmo escopo", decl->linha);
    }
    (*posicao)++;

    processarDeclVarChain(decl->filho3, tipoBase, pilha, posicao);
}

/*
 * processarListaDeclVar — percorre a cadeia AST_LISTA_DECL_VAR (uma
 * linha de declaração por nó) e, para cada uma, descobre o tipo na
 * cabeça da lista de nomes e delega para processarDeclVarChain.
 *
 * Usada tanto para a seção global quanto para declarações de bloco
 * (filho1 de AST_BLOCO).
 */
static void processarListaDeclVar(No *lista, PilhaTabela *pilha, int *posicao) {
    if (!lista) return;

    No *decl = lista->filho1; /* cabeça da linha: tem filho2 = Tipo */
    TipoVar tipoBase = (decl->filho2->tipo == AST_TIPO_INT) ? TIPO_INT : TIPO_CAR;
    processarDeclVarChain(decl, tipoBase, pilha, posicao);

    processarListaDeclVar(lista->filho2, pilha, posicao);
}

/* ------------------------------------------------------------------ */
/* Auxiliares: parâmetros formais (assinatura e inserção no corpo)    */
/* ------------------------------------------------------------------ */

/*
 * extrairParametro — lê um nó AST_PARAMETRO e devolve nome + tipo.
 * Igual à lógica de declaração de variável: vetor de parâmetro não tem
 * dimensão (filho2 do VETOR_DECL é NULL), mas isso não importa aqui,
 * só o nome (filho1) e o tipo básico (filho2 do PARAMETRO) interessam.
 */
static void extrairParametro(No *param, char **nomeOut, TipoVar *tipoOut) {
    No *alvo = param->filho1;
    TipoVar base = (param->filho2->tipo == AST_TIPO_INT) ? TIPO_INT : TIPO_CAR;

    if (alvo->tipo == AST_VETOR_DECL) {
        *nomeOut = alvo->filho1->valor;
        *tipoOut = (base == TIPO_INT) ? TIPO_VETOR_INT : TIPO_VETOR_CAR;
    } else {
        *nomeOut = alvo->valor;
        *tipoOut = base;
    }
}

/* ------------------------------------------------------------------ */
/* Auxiliares: funções (duas passadas) e chamadas de função           */
/* ------------------------------------------------------------------ */

/*
 * registrarAssinaturasFuncoes — PASSADA 1.
 *
 * Para cada função da lista, insere o cabeçalho (nome + tipo de
 * retorno) no escopo global e monta a assinatura de parâmetros, sem
 * entrar no corpo. Isso é o que permite chamadas para frente / recursão
 * mútua sem protótipo.
 */
static void registrarAssinaturasFuncoes(No *listaFunc, PilhaTabela *pilha) {
    if (!listaFunc) return;

    No *func = listaFunc->filho1; /* AST_FUNCAO: f1=ID, f2=TIPO, f3=PARAMS, f4=BLOCO */
    char *nome = func->filho1->valor;
    TipoVar tipoRetorno = (func->filho2->tipo == AST_TIPO_INT) ? TIPO_INT : TIPO_CAR;

    EntradaTabela *entrada = inserirFuncao(pilha, nome, tipoRetorno, func->linha);
    if (!entrada) {
        erroSemantico("Funcao redeclarada ou em conflito com simbolo global", func->linha);
    }

    No *p = func->filho3; /* AST_LISTA_PARAM ou NULL (funcao sem parametros) */
    while (p) {
        char *nomeParam;
        TipoVar tipoParam;
        extrairParametro(p->filho1, &nomeParam, &tipoParam);
        adicionarParametroAssinatura(entrada, nomeParam, tipoParam);
        p = p->filho2;
    }

    registrarAssinaturasFuncoes(listaFunc->filho2, pilha);
}

/*
 * analisarCorposFuncoes — PASSADA 2.
 *
 * Agora que toda função já está na tabela (passada 1), entra em cada
 * corpo de função: empilha UM único escopo, insere os parâmetros, e
 * insere as variáveis do bloco mais externo NESSE MESMO escopo (decisão
 * de design da tabela — ver tabela.h). Isso garante de graça a regra
 * "não pode variável local com mesmo nome de parâmetro no bloco mais
 * externo, mas pode em bloco aninhado": a checagem de redeclaração normal
 * já cobre o caso, sem código extra.
 */
static void analisarCorposFuncoes(No *listaFunc, PilhaTabela *pilha) {
    if (!listaFunc) return;

    No *func = listaFunc->filho1;
    char *nomeFunc = func->filho1->valor;
    EntradaTabela *entrada = buscarSimbolo(pilha, nomeFunc);
    /* não deveria nunca ser NULL aqui: a passada 1 já garantiu a inserção */

    bool dentroAnterior = dentroDeFuncao;
    TipoVar tipoRetornoAnterior = tipoRetornoAtual;
    dentroDeFuncao   = true;
    tipoRetornoAtual = entrada->tipo;

    empilharEscopo(pilha); /* escopo do corpo da função (parâmetros + bloco externo) */

    int posParam = 1;
    No *p = func->filho3;
    while (p) {
        char *nomeParam;
        TipoVar tipoParam;
        extrairParametro(p->filho1, &nomeParam, &tipoParam);
        if (!inserirParametro(pilha, nomeParam, tipoParam, posParam, p->filho1->linha)) {
            erroSemantico("Parametro redeclarado na mesma funcao", p->filho1->linha);
        }
        posParam++;
        p = p->filho2;
    }

    No *bloco = func->filho4; /* AST_BLOCO: f1=declaracoes locais, f2=comandos */
    int posVar = 1;
    processarListaDeclVar(bloco->filho1, pilha, &posVar); /* MESMO escopo dos parâmetros */
    analisarSemantica(bloco->filho2, pilha);               /* comandos do bloco externo  */

    desempilharEscopo(pilha);

    dentroDeFuncao   = dentroAnterior;
    tipoRetornoAtual = tipoRetornoAnterior;

    analisarCorposFuncoes(listaFunc->filho2, pilha);
}

/*
 * coletarArgs — "achata" a árvore de argumentos de uma chamada de
 * função (AST_LISTA_EXPR) num array, na ordem de escrita (esquerda
 * para a direita).
 *
 * Atenção: a gramática constrói essa lista de forma ESQUERDA-RECURSIVA
 * (ListExpr ',' Expr), então o argumento mais à esquerda fica mais
 * profundo na árvore (em filho1), e cada vírgula acrescenta o próximo
 * argumento em filho2. Por isso a recursão aqui desce por filho1
 * primeiro e só then anexa filho2 — preservando a ordem correta para
 * comparar posição a posição com os parâmetros formais.
 */
static int coletarArgs(No *lista, No **out) {
    if (!lista) return 0;
    if (lista->filho2 == NULL) {
        out[0] = lista->filho1; /* caso base: único argumento */
        return 1;
    }
    int n = coletarArgs(lista->filho1, out);
    out[n] = lista->filho2;
    return n + 1;
}

/* ------------------------------------------------------------------ */
/* Função principal recursiva                                         */
/* ------------------------------------------------------------------ */

TipoVar analisarSemantica(No *no, PilhaTabela *pilha) {
    if (!no) return -1;

    switch (no->tipo) {

        /*
         * AST_PROGRAMA — raiz da árvore.
         * filho1 = globais (AST_GLOBAIS ou NULL)
         * filho2 = funções (AST_LISTA_FUNC ou NULL)
         * filho3 = bloco principal
         */
        case AST_PROGRAMA: {
            if (no->filho1) { /* seção global existe */
                int pos = 1;
                processarListaDeclVar(no->filho1->filho1, pilha, &pos);
            }

            registrarAssinaturasFuncoes(no->filho2, pilha); /* passada 1 */
            analisarCorposFuncoes(no->filho2, pilha);        /* passada 2 */

            analisarSemantica(no->filho3, pilha); /* bloco principal */
            break;
        }

        /*
         * AST_BLOCO — usado para o bloco `principal` e para qualquer
         * bloco aninhado dentro de comandos (se/enquanto/bloco solto).
         * O bloco mais externo de uma função NÃO passa por aqui — é
         * tratado manualmente em analisarCorposFuncoes (mesmo escopo
         * dos parâmetros, ver comentário acima).
         */
        case AST_BLOCO: {
            empilharEscopo(pilha);
            int pos = 1;
            processarListaDeclVar(no->filho1, pilha, &pos);
            analisarSemantica(no->filho2, pilha);
            desempilharEscopo(pilha);
            break;
        }

        case AST_LISTA_COMANDO:
            analisarSemantica(no->filho1, pilha);
            analisarSemantica(no->filho2, pilha);
            break;

        /*
         * AST_IDENTIFICADOR — uso de uma variável/parâmetro em uma
         * expressão (não confundir com o identificador "cru" usado em
         * declarações, que nunca passa por aqui — é lido diretamente
         * pelos auxiliares de declaração acima).
         */
        case AST_IDENTIFICADOR: {
            EntradaTabela *sym = buscarSimbolo(pilha, no->valor);
            if (!sym) {
                erroSemantico("Variavel nao declarada no escopo atual", no->linha);
            }
            if (sym->categoria == CATEGORIA_FUNCAO) {
                erroSemantico("Identificador de funcao usado como variavel", no->linha);
            }
            return sym->tipo;
        }

        /*
         * AST_VETOR_ACESSO — v[i].
         * 'v' deve existir, ser vetor, e o índice deve ser do tipo int.
         * Retorna o TIPO BÁSICO do vetor (não o tipo vetor) — é assim
         * que a regra "pode operar entre v[i] e variável do mesmo tipo
         * básico do vetor" sai de graça das checagens normais abaixo.
         */
        case AST_VETOR_ACESSO: {
            EntradaTabela *sym = buscarSimbolo(pilha, no->filho1->valor);
            if (!sym) {
                erroSemantico("Variavel nao declarada no escopo atual", no->linha);
            }
            if (sym->categoria == CATEGORIA_FUNCAO) {
                erroSemantico("Identificador de funcao usado como vetor", no->linha);
            }
            if (sym->tipo != TIPO_VETOR_INT && sym->tipo != TIPO_VETOR_CAR) {
                erroSemantico("Tentativa de indexar uma variavel que nao e vetor", no->linha);
            }

            TipoVar tipoIndice = analisarSemantica(no->filho2, pilha);
            if (tipoIndice != TIPO_INT) {
                erroSemantico("Indice de vetor deve ser do tipo int", no->linha);
            }

            return (sym->tipo == TIPO_VETOR_INT) ? TIPO_INT : TIPO_CAR;
        }

        /*
         * AST_CHAMADA_FUNC — f(args...).
         * Verifica: existe e é função; número de argumentos bate com o
         * número de parâmetros formais; tipo de cada argumento bate, na
         * ordem, com o tipo do parâmetro formal correspondente.
         */
        case AST_CHAMADA_FUNC: {
            EntradaTabela *sym = buscarSimbolo(pilha, no->filho1->valor);
            if (!sym) {
                erroSemantico("Funcao nao declarada", no->linha);
            }
            if (sym->categoria != CATEGORIA_FUNCAO) {
                erroSemantico("Identificador usado como funcao nao e uma funcao", no->linha);
            }

            No *argsNo[MAX_ARGS];
            int numArgs = no->filho2 ? coletarArgs(no->filho2, argsNo) : 0;

            if (numArgs != sym->numParametros) {
                erroSemantico("Numero de argumentos nao corresponde ao numero de parametros", no->linha);
            }

            ParametroAssinatura *paramFormal = sym->assinatura;
            for (int i = 0; i < numArgs; i++) {
                TipoVar tipoArg = analisarSemantica(argsNo[i], pilha);
                if (tipoArg != paramFormal->tipo) {
                    erroSemantico("Tipo de argumento incompativel com o parametro formal", no->linha);
                }
                paramFormal = paramFormal->prox;
            }

            return sym->tipo; /* tipo de retorno da função */
        }

        /* Constantes têm tipo fixo */
        case AST_INTCONST:
            return TIPO_INT;

        case AST_CARCONST:
            return TIPO_CAR;

        /*
         * AST_ATRIB — lvalue = expr.
         * Cobre variável=variável, vetor=vetor (cópia de endereço,
         * mesmo tipo básico) e variável=v[i] / v[i]=variável, todos
         * pela mesma checagem de igualdade de tipo (o enum TipoVar já
         * distingue escalar de vetor).
         */
        case AST_ATRIB: {
            TipoVar tipoEsq = analisarSemantica(no->filho1, pilha);
            TipoVar tipoDir = analisarSemantica(no->filho2, pilha);
            if (tipoEsq != tipoDir) {
                erroSemantico("Atribuicao entre tipos incompativeis", no->linha);
            }
            return tipoEsq;
        }

        /*
         * Operadores aritméticos: exigem os dois operandos int.
         * Vetor e car são rejeitados automaticamente pela comparação.
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

        case AST_NEG:
        case AST_NAO: {
            TipoVar tipo = analisarSemantica(no->filho1, pilha);
            if (tipo != TIPO_INT) {
                erroSemantico("Operadores unarios ('-', '!') exigem operando do tipo int", no->linha);
            }
            return TIPO_INT;
        }

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
         * Operadores relacionais: exigem operandos do MESMO tipo.
         * Resultado é tratado como int (G-V2 não tem booleano explícito).
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
            return TIPO_INT;
        }

        case AST_CMD_EXPR:
        case AST_CMD_ESCREVA_EXPR:
        case AST_CMD_LEIA:
            analisarSemantica(no->filho1, pilha);
            break;

        /*
         * AST_CMD_RETORNE — retorne expr;
         * Só é válido dentro de uma função, e o tipo da expressão deve
         * bater com o tipo de retorno declarado para a função atual.
         */
        case AST_CMD_RETORNE: {
            if (!dentroDeFuncao) {
                erroSemantico("Comando 'retorne' usado fora de uma funcao", no->linha);
            }
            TipoVar tipoExpr = analisarSemantica(no->filho1, pilha);
            if (tipoExpr != tipoRetornoAtual) {
                erroSemantico("Tipo de retorno incompativel com o tipo da funcao", no->linha);
            }
            return tipoExpr;
        }

        case AST_CMD_SE:
        case AST_CMD_ENQUANTO: {
            TipoVar tipoCond = analisarSemantica(no->filho1, pilha);
            if (tipoCond != TIPO_INT) {
                erroSemantico("A condicao deve resultar em tipo int", no->linha);
            }
            analisarSemantica(no->filho2, pilha);
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
            return -1;

        /*
         * Caso padrão: nós não tratados explicitamente (não deveria
         * ocorrer para nós de declaração/parâmetro/função, que são
         * sempre processados pelos auxiliares dedicados acima e nunca
         * chegam aqui via recursão genérica).
         */
        default:
            analisarSemantica(no->filho1, pilha);
            analisarSemantica(no->filho2, pilha);
            analisarSemantica(no->filho3, pilha);
            break;
    }
    return -1;
}
