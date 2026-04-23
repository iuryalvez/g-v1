/*
 * gerador.c — Geração de Código Assembly MIPS
 *
 * Percorre a AST (já validada pelo analisador semântico) e emite
 * instruções MIPS para cada construção da linguagem G-V1.
 *
 * O código gerado segue as convenções de chamada MIPS simplificadas:
 *   - variáveis locais ficam na pilha, acessadas via $fp + offset
 *   - expressões são avaliadas usando registradores temporários $t0-$t9
 *   - operações de I/O usam syscalls do SPIM/MARS
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tabela.h"
#include "gerador.h"

/* =====================================================================
 * Gerenciamento de registradores temporários
 *
 * Usamos uma pilha simples de registradores $t0-$t9 para guardar
 * resultados intermediários de expressões. Cada vez que precisamos de
 * um temporário, "alocamos" um; quando terminamos, "liberamos".
 * ===================================================================== */

static int regsUsados = 0; /* quantos temporários estão em uso */

/* Retorna o número do próximo registrador temporário disponível */
static int alocarReg() {
    if (regsUsados >= 10) {
        fprintf(stderr, "Erro interno: sem registradores temporários disponíveis\n");
        exit(1);
    }
    return regsUsados++; /* retorna $t0, $t1, $t2, ... */
}

/* Libera o registrador temporário mais recentemente alocado */
static void liberarReg() {
    if (regsUsados > 0) regsUsados--;
}

/* =====================================================================
 * Gerador de rótulos únicos (labels)
 *
 * Comandos como 'se' e 'enquanto' precisam de rótulos no assembly para
 * indicar para onde o programa deve saltar. Ex: label_0, label_1, ...
 * ===================================================================== */

static int contadorLabel = 0;

/* Retorna um novo número de label único a cada chamada */
static int novoLabel() {
    return contadorLabel++;
}

/* =====================================================================
 * Pilha de tabelas para geração de código
 *
 * Semelhante à tabela de símbolos usada na análise semântica, mas
 * aqui cada entrada também guarda o offset da variável na pilha MIPS.
 * ===================================================================== */

static PilhaGerador *pilhaGen = NULL;

/* Inicializa a pilha do gerador */
static void iniciarPilhaGen() {
    pilhaGen = (PilhaGerador *)malloc(sizeof(PilhaGerador));
    pilhaGen->topo = NULL;
    pilhaGen->offsetAtual = 0;
}

/* Abre um novo escopo na pilha do gerador */
static void empilharEscopoGen() {
    TabelaGerador *nova = (TabelaGerador *)malloc(sizeof(TabelaGerador));
    nova->entradas  = NULL;
    nova->tamanho   = 0;
    nova->anterior  = pilhaGen->topo;
    pilhaGen->topo  = nova;
}

/*
 * Insere uma variável no escopo atual do gerador.
 * O offset é calculado automaticamente com base no tamanho do tipo.
 * Retorna o offset atribuído à variável.
 */
static int inserirVarGen(const char *nome, TipoVar tipo) {
    EntradaGerador *nova = (EntradaGerador *)malloc(sizeof(EntradaGerador));
    nova->nome  = strdup(nome);
    nova->tipo  = tipo;

    /* int ocupa 4 bytes; car ocupa 1 byte mas alinhamos em 4 por simplicidade */
    int tamanho = (tipo == TIPO_INT) ? 4 : 4;

    /* offset negativo: variáveis crescem para baixo na pilha */
    pilhaGen->offsetAtual -= tamanho;
    nova->offset = pilhaGen->offsetAtual;
    pilhaGen->topo->tamanho += tamanho;

    nova->prox = pilhaGen->topo->entradas;
    pilhaGen->topo->entradas = nova;
    return nova->offset;
}

/*
 * Busca uma variável pelo nome, percorrendo todos os escopos.
 * Retorna a entrada encontrada ou NULL.
 */
static EntradaGerador *buscarVarGen(const char *nome) {
    TabelaGerador *escopo = pilhaGen->topo;
    while (escopo) {
        EntradaGerador *e = escopo->entradas;
        while (e) {
            if (strcmp(e->nome, nome) == 0) return e;
            e = e->prox;
        }
        escopo = escopo->anterior;
    }
    return NULL;
}

/* Fecha o escopo atual e retorna quantos bytes foram alocados nele */
static int desempilharEscopoGen() {
    if (!pilhaGen->topo) return 0;
    TabelaGerador *removida = pilhaGen->topo;
    int tamanho = removida->tamanho;

    /* libera entradas */
    EntradaGerador *e = removida->entradas;
    while (e) {
        EntradaGerador *prox = e->prox;
        free(e->nome);
        free(e);
        e = prox;
    }

    pilhaGen->topo = removida->anterior;
    pilhaGen->offsetAtual += tamanho; /* devolve o espaço na pilha */
    free(removida);
    return tamanho;
}

/* =====================================================================
 * Funções de geração de expressões
 *
 * Cada função recebe um nó da AST e emite as instruções MIPS para
 * avaliar aquela expressão. O resultado fica no registrador $tREG,
 * onde REG é o valor retornado pela função.
 * ===================================================================== */

/* Declaração antecipada para recursão mútua */
static int gerarExpr(No *no, FILE *out);
static void gerarComando(No *no, FILE *out);

/*
 * gerarExpr — gera código para uma expressão e retorna o número do
 * registrador temporário onde o resultado ficou armazenado.
 *
 * Exemplo: para "x + 3", gera:
 *   lw   $t0, offset_x($fp)   # carrega x em $t0
 *   li   $t1, 3               # carrega constante 3 em $t1
 *   add  $t2, $t0, $t1        # soma e guarda em $t2
 * e retorna 2 (índice do $t2).
 */
static int gerarExpr(No *no, FILE *out) {
    if (!no) return -1;

    int reg, regEsq, regDir;

    switch (no->tipo) {

        /* --- Constante inteira: li $tN, valor --- */
        case AST_INTCONST:
            reg = alocarReg();
            fprintf(out, "    li   $t%d, %s\n", reg, no->valor);
            return reg;

        /* --- Constante de caractere: li $tN, valor_ascii --- */
        case AST_CARCONST: {
            reg = alocarReg();
            /* CARCONST vem como 'c' — extraímos o caractere do meio */
            char c = no->valor[1];
            if (c == '\\') {
                /* sequências de escape */
                char esc = no->valor[2];
                switch (esc) {
                    case 'n': c = '\n'; break;
                    case 't': c = '\t'; break;
                    case '0': c = '\0'; break;
                    default:  c = esc;  break;
                }
            }
            fprintf(out, "    li   $t%d, %d\n", reg, (int)c);
            return reg;
        }

        /* --- Identificador: carrega da pilha --- */
        case AST_IDENTIFICADOR: {
            EntradaGerador *entrada = buscarVarGen(no->valor);
            reg = alocarReg();
            if (entrada->tipo == TIPO_INT) {
                /* int: carrega word (4 bytes) */
                fprintf(out, "    lw   $t%d, %d($fp)\n", reg, entrada->offset);
            } else {
                /* car: carrega byte sem sinal */
                fprintf(out, "    lb   $t%d, %d($fp)\n", reg, entrada->offset);
            }
            return reg;
        }

        /* --- Atribuição: avalia lado direito e salva na variável --- */
        case AST_ATRIB: {
            regDir = gerarExpr(no->filho2, out); /* avalia o lado direito */
            EntradaGerador *entrada = buscarVarGen(no->filho1->valor);
            if (entrada->tipo == TIPO_INT) {
                fprintf(out, "    sw   $t%d, %d($fp)\n", regDir, entrada->offset);
            } else {
                fprintf(out, "    sb   $t%d, %d($fp)\n", regDir, entrada->offset);
            }
            reg = regDir; /* o resultado da atribuição é o valor atribuído */
            return reg;
        }

        /* --- Operações aritméticas binárias --- */
        case AST_SOMA:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            reg = alocarReg();
            fprintf(out, "    add  $t%d, $t%d, $t%d\n", reg, regEsq, regDir);
            liberarReg(); liberarReg(); /* libera regDir e regEsq */
            return reg;

        case AST_SUB:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            reg = alocarReg();
            fprintf(out, "    sub  $t%d, $t%d, $t%d\n", reg, regEsq, regDir);
            liberarReg(); liberarReg();
            return reg;

        case AST_MUL:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            reg = alocarReg();
            /* mul usa registrador especial HI/LO; mflo move resultado para $tN */
            fprintf(out, "    mul  $t%d, $t%d, $t%d\n", reg, regEsq, regDir);
            liberarReg(); liberarReg();
            return reg;

        case AST_DIV:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            reg = alocarReg();
            fprintf(out, "    div  $t%d, $t%d\n", regEsq, regDir);
            fprintf(out, "    mflo $t%d\n", reg); /* quociente fica em LO */
            liberarReg(); liberarReg();
            return reg;

        /* --- Negação unária: sub $tN, $zero, $tM --- */
        case AST_NEG:
            regEsq = gerarExpr(no->filho1, out);
            reg = alocarReg();
            fprintf(out, "    sub  $t%d, $zero, $t%d\n", reg, regEsq);
            liberarReg();
            return reg;

        /* --- Negação lógica: !x → x == 0 ? 1 : 0 --- */
        case AST_NAO:
            regEsq = gerarExpr(no->filho1, out);
            reg = alocarReg();
            fprintf(out, "    seq  $t%d, $t%d, $zero\n", reg, regEsq);
            liberarReg();
            return reg;

        /* --- Operadores relacionais --- */
        case AST_MENOR:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            reg = alocarReg();
            fprintf(out, "    slt  $t%d, $t%d, $t%d\n", reg, regEsq, regDir);
            liberarReg(); liberarReg();
            return reg;

        case AST_MAIOR:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            reg = alocarReg();
            /* a > b  ≡  b < a */
            fprintf(out, "    slt  $t%d, $t%d, $t%d\n", reg, regDir, regEsq);
            liberarReg(); liberarReg();
            return reg;

        case AST_MENORIGUAL:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            reg = alocarReg();
            /* a <= b  ≡  !(b < a) */
            fprintf(out, "    slt  $t%d, $t%d, $t%d\n", reg, regDir, regEsq);
            fprintf(out, "    xori $t%d, $t%d, 1\n",    reg, reg);
            liberarReg(); liberarReg();
            return reg;

        case AST_MAIORIGUAL:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            reg = alocarReg();
            /* a >= b  ≡  !(a < b) */
            fprintf(out, "    slt  $t%d, $t%d, $t%d\n", reg, regEsq, regDir);
            fprintf(out, "    xori $t%d, $t%d, 1\n",    reg, reg);
            liberarReg(); liberarReg();
            return reg;

        case AST_IGUAL:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            reg = alocarReg();
            fprintf(out, "    seq  $t%d, $t%d, $t%d\n", reg, regEsq, regDir);
            liberarReg(); liberarReg();
            return reg;

        case AST_DIFERENTE:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            reg = alocarReg();
            fprintf(out, "    sne  $t%d, $t%d, $t%d\n", reg, regEsq, regDir);
            liberarReg(); liberarReg();
            return reg;

        /* --- Operadores lógicos --- */
        case AST_E:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            reg = alocarReg();
            fprintf(out, "    and  $t%d, $t%d, $t%d\n", reg, regEsq, regDir);
            liberarReg(); liberarReg();
            return reg;

        case AST_OU:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            reg = alocarReg();
            fprintf(out, "    or   $t%d, $t%d, $t%d\n", reg, regEsq, regDir);
            liberarReg(); liberarReg();
            return reg;

        default:
            return -1;
    }
}

/* =====================================================================
 * Funções de geração de declarações
 * ===================================================================== */

/*
 * gerarDeclaracoes — percorre a lista de declarações de variáveis,
 * insere cada variável na tabela do gerador e reserva espaço na pilha.
 * Retorna o total de bytes a reservar neste bloco.
 */
static int gerarDeclaracoes(No *no) {
    if (!no) return 0;
    int total = 0;

    if (no->tipo == AST_LISTA_DECL_VAR) {
        total += gerarDeclaracoes(no->filho1); /* processa declaração atual */
        total += gerarDeclaracoes(no->filho2); /* processa restante da lista */
        return total;
    }

    if (no->tipo == AST_DECL_VAR) {
        TipoVar tipo = (no->filho2->tipo == AST_TIPO_INT) ? TIPO_INT : TIPO_CAR;
        inserirVarGen(no->filho1->valor, tipo);
        total += 4; /* todos os tipos ocupam 4 bytes na pilha (alinhamento) */

        /* variáveis extras separadas por vírgula (filho3) */
        No *extra = no->filho3;
        while (extra) {
            inserirVarGen(extra->filho1->valor, tipo);
            total += 4;
            extra = extra->filho2;
        }
    }

    return total;
}

/* =====================================================================
 * Geração de comandos
 * ===================================================================== */

/*
 * gerarBloco — gera o código para um bloco { declarações } { comandos }.
 *
 * Ao entrar: reserva espaço na pilha para as variáveis locais.
 * Ao sair:   libera o espaço, restaurando o $sp.
 */
static void gerarBloco(No *no, FILE *out) {
    if (!no || no->tipo != AST_BLOCO) return;

    empilharEscopoGen();

    /* conta os bytes necessários para as variáveis deste bloco */
    int bytes = gerarDeclaracoes(no->filho1);

    if (bytes > 0) {
        fprintf(out, "    # abre escopo: reserva %d bytes na pilha\n", bytes);
        fprintf(out, "    addi $sp, $sp, -%d\n", bytes);
    }

    /* gera os comandos do bloco */
    gerarComando(no->filho2, out);

    /* fecha o escopo: devolve o espaço na pilha */
    int liberado = desempilharEscopoGen();
    if (liberado > 0) {
        fprintf(out, "    # fecha escopo: libera %d bytes da pilha\n", liberado);
        fprintf(out, "    addi $sp, $sp, %d\n", liberado);
    }
}

/*
 * gerarComando — gera o código para um comando ou lista de comandos.
 */
static void gerarComando(No *no, FILE *out) {
    if (!no) return;

    switch (no->tipo) {

        /* Lista: gera o comando atual e depois os demais */
        case AST_LISTA_COMANDO:
            gerarComando(no->filho1, out);
            gerarComando(no->filho2, out);
            break;

        /* Comando vazio: não gera nada */
        case AST_CMD_VAZIO:
            break;

        /* Expressão como comando: avalia e descarta o resultado */
        case AST_CMD_EXPR: {
            int reg = gerarExpr(no->filho1, out);
            if (reg >= 0) liberarReg();
            break;
        }

        /*
         * leia id; — lê um valor do teclado e armazena na variável.
         * Syscall 5 = lê inteiro; syscall 12 = lê caractere.
         * O resultado da syscall é sempre retornado em $v0.
         */
        case AST_CMD_LEIA: {
            EntradaGerador *entrada = buscarVarGen(no->filho1->valor);
            if (entrada->tipo == TIPO_INT) {
                fprintf(out, "    # leia int: %s\n", no->filho1->valor);
                fprintf(out, "    li   $v0, 5\n");       /* syscall 5 = lê int */
                fprintf(out, "    syscall\n");
                fprintf(out, "    sw   $v0, %d($fp)\n", entrada->offset);
            } else {
                fprintf(out, "    # leia car: %s\n", no->filho1->valor);
                fprintf(out, "    li   $v0, 12\n");      /* syscall 12 = lê char */
                fprintf(out, "    syscall\n");
                fprintf(out, "    sb   $v0, %d($fp)\n", entrada->offset);
            }
            break;
        }

        /*
         * escreva expr; — imprime o valor de uma expressão.
         * Syscall 1 = imprime inteiro; syscall 11 = imprime caractere.
         */
        case AST_CMD_ESCREVA_EXPR: {
            int reg = gerarExpr(no->filho1, out);
            /* precisamos saber o tipo para escolher a syscall correta */
            int syscallNum = 1; /* padrão: imprime int */
            if (no->filho1->tipo == AST_IDENTIFICADOR) {
                EntradaGerador *e = buscarVarGen(no->filho1->valor);
                if (e && e->tipo == TIPO_CAR) syscallNum = 11;
            } else if (no->filho1->tipo == AST_CARCONST) {
                syscallNum = 11;
            }
            fprintf(out, "    # escreva expressao\n");
            fprintf(out, "    move $a0, $t%d\n", reg);
            fprintf(out, "    li   $v0, %d\n", syscallNum);
            fprintf(out, "    syscall\n");
            liberarReg();
            break;
        }

        /*
         * escreva "string"; — imprime uma string literal.
         * Syscall 4 = imprime string. A string precisa estar na seção .data.
         * Geramos um label único para cada string.
         */
        case AST_CMD_ESCREVA_STR: {
            int id = novoLabel();
            /* guarda a string para emitir na seção .data depois */
            /* solução simples: emitimos inline usando .data temporário */
            fprintf(out, "    # escreva string\n");
            fprintf(out, "    la   $a0, _str%d\n", id);
            fprintf(out, "    li   $v0, 4\n");
            fprintf(out, "    syscall\n");
            /* emite a string em uma seção .data inline */
            fprintf(out, "    .data\n");
            /* remove as aspas do valor: "hello" → hello */
            char *str = no->valor;
            int len = strlen(str);
            fprintf(out, "_str%d: .asciiz %.*s\n", id, len, str);
            fprintf(out, "    .text\n");
            break;
        }

        /*
         * novalinha; — imprime uma quebra de linha.
         * Syscall 11 imprime o caractere cujo código ASCII está em $a0.
         * '\n' tem código ASCII 10.
         */
        case AST_CMD_NOVALINHA:
            fprintf(out, "    # novalinha\n");
            fprintf(out, "    li   $a0, 10\n");  /* ASCII 10 = '\n' */
            fprintf(out, "    li   $v0, 11\n");  /* syscall 11 = imprime char */
            fprintf(out, "    syscall\n");
            break;

        /*
         * se (cond) entao cmd fimse
         *
         * Estrutura gerada:
         *   <avalia condição>
         *   beqz $tN, label_fim   # se falso, pula para o fim
         *   <código do then>
         * label_fim:
         */
        case AST_CMD_SE: {
            int labelFim = novoLabel();
            int regCond = gerarExpr(no->filho1, out);
            fprintf(out, "    # se: pula se condicao falsa\n");
            fprintf(out, "    beqz $t%d, _label%d\n", regCond, labelFim);
            liberarReg();
            gerarComando(no->filho2, out);
            fprintf(out, "_label%d:\n", labelFim);
            break;
        }

        /*
         * se (cond) entao cmd senao cmd fimse
         *
         * Estrutura gerada:
         *   <avalia condição>
         *   beqz $tN, label_senao  # se falso, vai para o senao
         *   <código do then>
         *   j    label_fim          # pula o senao
         * label_senao:
         *   <código do senao>
         * label_fim:
         */
        case AST_CMD_SE_SENAO: {
            int labelSenao = novoLabel();
            int labelFim   = novoLabel();
            int regCond = gerarExpr(no->filho1, out);
            fprintf(out, "    # se-senao\n");
            fprintf(out, "    beqz $t%d, _label%d\n", regCond, labelSenao);
            liberarReg();
            gerarComando(no->filho2, out); /* then */
            fprintf(out, "    j    _label%d\n", labelFim);
            fprintf(out, "_label%d:\n", labelSenao);
            gerarComando(no->filho3, out); /* senao */
            fprintf(out, "_label%d:\n", labelFim);
            break;
        }

        /*
         * enquanto (cond) cmd
         *
         * Estrutura gerada:
         * label_inicio:
         *   <avalia condição>
         *   beqz $tN, label_fim  # se falso, sai do laço
         *   <código do corpo>
         *   j    label_inicio    # volta para testar novamente
         * label_fim:
         */
        case AST_CMD_ENQUANTO: {
            int labelInicio = novoLabel();
            int labelFim    = novoLabel();
            fprintf(out, "_label%d:\n", labelInicio);
            int regCond = gerarExpr(no->filho1, out);
            fprintf(out, "    # enquanto: pula se condicao falsa\n");
            fprintf(out, "    beqz $t%d, _label%d\n", regCond, labelFim);
            liberarReg();
            gerarComando(no->filho2, out);
            fprintf(out, "    j    _label%d\n", labelInicio);
            fprintf(out, "_label%d:\n", labelFim);
            break;
        }

        /* Bloco aninhado como comando */
        case AST_BLOCO:
            gerarBloco(no, out);
            break;

        default:
            break;
    }
}

/* =====================================================================
 * Ponto de entrada da geração de código
 * ===================================================================== */

/*
 * gerarCodigo — função principal chamada após a análise semântica.
 *
 * Emite o cabeçalho do arquivo assembly MIPS e inicia o percurso na AST.
 * O código gerado pode ser executado no simulador SPIM ou MARS.
 */
void gerarCodigo(No *raiz, FILE *saida) {
    iniciarPilhaGen();

    /* === Cabeçalho do arquivo assembly === */
    fprintf(saida, "# Código MIPS gerado pelo compilador G-V1\n");
    fprintf(saida, "    .text\n");
    fprintf(saida, "    .globl main\n");
    fprintf(saida, "main:\n");

    /*
     * Prólogo: configura o frame pointer ($fp).
     * $fp aponta para o topo da pilha no início do programa.
     * Todas as variáveis serão acessadas como $fp + offset (offset negativo).
     */
    fprintf(saida, "    # prologo: configura frame pointer\n");
    fprintf(saida, "    move $fp, $sp\n");

    /* Percorre a AST a partir do bloco principal */
    if (raiz && raiz->tipo == AST_PROGRAMA) {
        gerarBloco(raiz->filho1, saida);
    }

    /*
     * Epílogo: encerra o programa com a syscall 10.
     * Syscall 10 = exit (finaliza o programa no SPIM/MARS).
     */
    fprintf(saida, "    # epilogo: encerra o programa\n");
    fprintf(saida, "    li   $v0, 10\n");
    fprintf(saida, "    syscall\n");

    free(pilhaGen);
}
