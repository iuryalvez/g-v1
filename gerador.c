/*
 * gerador.c — Geração de Código Assembly MIPS (G-V2)
 *
 * Percorre a AST (já validada pelo analisador semântico) e emite
 * instruções MIPS para cada construção da linguagem G-V2: variáveis
 * globais, funções (com parâmetros e retorno), vetores e os mesmos
 * comandos/expressões já existentes em G-V1.
 *
 * Ver gerador.h para a explicação completa do layout de memória, da
 * convenção de chamada de função e do esquema uniforme de vetores.
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
 * Pilha simples de registradores $t0-$t9. REGRA DE OURO seguida em
 * TODO lugar deste arquivo: ao combinar dois (ou um) operandos em um
 * resultado, sempre LIBERAMOS os registradores dos operandos ANTES de
 * ALOCAR o registrador do resultado.
 *
 * Isso evita um bug sutil que existia no gerador de G-V1: se o
 * resultado é alocado ANTES de liberar os operandos, o número do
 * registrador do resultado fica "acima" do topo real da pilha lógica
 * (regsUsados). Em expressões com os DOIS lados "fundos" — por exemplo
 * (a+b)+(c+d), onde os dois operandos da soma externa também são somas
 * — o resultado da subexpressão esquerda fica "esquecido" num
 * registrador fora do controle do contador, e a avaliação da
 * subexpressão direita pode reutilizar (sobrescrever) esse mesmo
 * registrador antes que o valor da esquerda seja consumido. Liberando
 * primeiro, o resultado sempre cai exatamente no registrador que tinha
 * acabado de ficar livre, sem deixar nada "órfão" acima do contador.
 * ===================================================================== */

static int regsUsados = 0; /* quantos temporários estão em uso agora */

/* Retorna o número do próximo registrador temporário disponível */
static int alocarReg(void) {
    if (regsUsados >= 10) {
        fprintf(stderr, "Erro interno: sem registradores temporarios disponiveis\n");
        exit(1);
    }
    return regsUsados++; /* retorna $t0, $t1, $t2, ... */
}

/* Libera o registrador temporário mais recentemente alocado */
static void liberarReg(void) {
    if (regsUsados > 0) regsUsados--;
}

/* =====================================================================
 * Gerador de rótulos únicos (labels)
 * ===================================================================== */

static int contadorLabel = 0;

static int novoLabel(void) {
    return contadorLabel++;
}

/* =====================================================================
 * Tabela de variáveis LOCAIS (parâmetros + variáveis de bloco)
 *
 * Pilha de escopos própria do gerador — independente da tabela usada na
 * análise semântica (aquela já foi liberada antes do gerador começar).
 * É reiniciada do zero a cada função (G-V2 não tem funções aninhadas, e
 * variáveis locais nunca "escapam" de uma função para outra).
 * ===================================================================== */

static PilhaGerador *pilhaGen = NULL;

static void iniciarPilhaGen(void) {
    pilhaGen = (PilhaGerador *)malloc(sizeof(PilhaGerador));
    pilhaGen->topo = NULL;
    pilhaGen->offsetAtual = 0;
}

static void empilharEscopoGen(void) {
    TabelaGerador *nova = (TabelaGerador *)malloc(sizeof(TabelaGerador));
    nova->entradas = NULL;
    nova->tamanho  = 0;
    nova->anterior = pilhaGen->topo;
    pilhaGen->topo = nova;
}

/*
 * removerEscopoGen — remove o escopo do topo e devolve quantos bytes
 * ele tinha reservado (para o chamador desfazer a reserva na pilha).
 */
static int removerEscopoGen(void) {
    if (!pilhaGen->topo) return 0;
    TabelaGerador *removida = pilhaGen->topo;
    int tamanho = removida->tamanho;

    EntradaGerador *e = removida->entradas;
    while (e) {
        EntradaGerador *prox = e->prox;
        free(e->nome);
        free(e);
        e = prox;
    }

    pilhaGen->topo = removida->anterior;
    free(removida);
    return tamanho;
}

/*
 * inserirVarGen — registra uma variável/parâmetro local no escopo
 * atual, com um deslocamento (offset) já calculado por quem chama
 * (offsets de parâmetro são positivos e vêm de uma fórmula fixa;
 * offsets de variável de bloco são negativos e crescem a cada inserção
 * — ver gerarParametros/gerarDeclaracoesLocais).
 */
static void inserirVarGen(const char *nome, TipoVar tipo, int offset) {
    EntradaGerador *nova = (EntradaGerador *)malloc(sizeof(EntradaGerador));
    nova->nome   = strdup(nome);
    nova->tipo   = tipo;
    nova->offset = offset;
    nova->global = 0;
    nova->prox   = pilhaGen->topo->entradas;
    pilhaGen->topo->entradas = nova;
}

/* =====================================================================
 * Tabela de variáveis GLOBAIS (lista simples, sem escopos)
 *
 * Variáveis globais existem do início ao fim da execução, então não
 * precisam de uma pilha — uma lista é suficiente. Todas são acessadas
 * a partir de $s1 (carregado uma única vez no início do programa).
 * ===================================================================== */

static EntradaGerador *tabelaGlobaisGen = NULL;

static void inserirGlobalGen(const char *nome, TipoVar tipo, int offset) {
    EntradaGerador *nova = (EntradaGerador *)malloc(sizeof(EntradaGerador));
    nova->nome   = strdup(nome);
    nova->tipo   = tipo;
    nova->offset = offset;
    nova->global = 1;
    nova->prox   = tabelaGlobaisGen;
    tabelaGlobaisGen = nova;
}

/*
 * buscarVarGen — procura uma variável/parâmetro primeiro nos escopos
 * locais (do mais interno ao mais externo, dentro da função atual) e,
 * se não achar, nas globais. Isso é exatamente o sombreamento de nomes
 * que a análise semântica já validou — aqui só reproduzimos a mesma
 * ordem de busca para gerar o endereço certo.
 */
static EntradaGerador *buscarVarGen(const char *nome) {
    if (pilhaGen) {
        TabelaGerador *escopo = pilhaGen->topo;
        while (escopo) {
            EntradaGerador *e = escopo->entradas;
            while (e) {
                if (strcmp(e->nome, nome) == 0) return e;
                e = e->prox;
            }
            escopo = escopo->anterior;
        }
    }

    EntradaGerador *g = tabelaGlobaisGen;
    while (g) {
        if (strcmp(g->nome, nome) == 0) return g;
        g = g->prox;
    }

    /* não deveria acontecer: a análise semântica já garantiu que toda
       variável usada existe em algum escopo visível */
    fprintf(stderr, "Erro interno do gerador: variavel '%s' nao encontrada\n", nome);
    exit(1);
}

/* =====================================================================
 * Tabela de FUNÇÕES (nome + tipo de retorno)
 *
 * Usada só para escolher a syscall certa (int vs car) quando uma
 * chamada de função aparece dentro de "escreva"/"leia".
 * ===================================================================== */

static FuncaoGerador *tabelaFuncoesGen = NULL;

static void inserirFuncaoGen(const char *nome, TipoVar tipoRetorno) {
    FuncaoGerador *nova = (FuncaoGerador *)malloc(sizeof(FuncaoGerador));
    nova->nome        = strdup(nome);
    nova->tipoRetorno  = tipoRetorno;
    nova->prox         = tabelaFuncoesGen;
    tabelaFuncoesGen   = nova;
}

static TipoVar buscarTipoRetornoFuncao(const char *nome) {
    FuncaoGerador *f = tabelaFuncoesGen;
    while (f) {
        if (strcmp(f->nome, nome) == 0) return f->tipoRetorno;
        f = f->prox;
    }
    fprintf(stderr, "Erro interno do gerador: funcao '%s' nao encontrada\n", nome);
    exit(1);
}

/* =====================================================================
 * tipoExpressao — descobre o TIPO de uma expressão (TIPO_INT/TIPO_CAR)
 * sem gerar nenhum código, só para escolher a syscall certa em
 * "escreva"/"leia". Não faz checagem de erro (a análise semântica já
 * garantiu que o programa é válido) — é uma versão "enxuta" da lógica
 * de tipos de semantico.c.
 * ===================================================================== */

static TipoVar tipoExpressao(No *no) {
    switch (no->tipo) {
        case AST_IDENTIFICADOR:
            return buscarVarGen(no->valor)->tipo;
        case AST_VETOR_ACESSO: {
            TipoVar tipoVetor = buscarVarGen(no->filho1->valor)->tipo;
            return (tipoVetor == TIPO_VETOR_CAR) ? TIPO_CAR : TIPO_INT;
        }
        case AST_CHAMADA_FUNC:
            return buscarTipoRetornoFuncao(no->filho1->valor);
        case AST_CARCONST:
            return TIPO_CAR;
        case AST_ATRIB:
            return tipoExpressao(no->filho1);
        default:
            /* todo o resto (aritmetica, logica, relacionais, unarios,
               INTCONST) sempre resulta em int na linguagem G-V2 */
            return TIPO_INT;
    }
}

/* Declarações antecipadas para recursão mútua entre as funções abaixo */
static int  gerarExpr(No *no, FILE *out);
static void gerarComando(No *no, FILE *out);
static void gerarBloco(No *no, FILE *out);
static int  coletarArgsChamada(No *lista, No **out);

/* =====================================================================
 * gerarEnderecoVetor — calcula o endereço de um elemento v[indice].
 *
 * Funciona de forma IDÊNTICA para vetor local, global ou recebido por
 * parâmetro: em todos os casos o slot da variável guarda um PONTEIRO
 * para os dados (ver gerador.h) — só a BASE usada para achar o slot
 * muda ($s1 para global, $fp para local/parâmetro).
 *
 * Devolve o registrador com o endereço calculado e, por 'tipoElemento',
 * o tipo do elemento (TIPO_INT ou TIPO_CAR) para quem chamou escolher
 * lw/sw (4 bytes) ou lb/sb (1 byte).
 * ===================================================================== */

static int gerarEnderecoVetor(No *acesso, FILE *out, TipoVar *tipoElemento) {
    EntradaGerador *entrada = buscarVarGen(acesso->filho1->valor);
    const char *base = entrada->global ? "$s1" : "$fp";

    int regBase = alocarReg();
    fprintf(out, "    lw    $t%d, %d(%s)         # ponteiro do vetor '%s'\n",
            regBase, entrada->offset, base, acesso->filho1->valor);

    int regIndice = gerarExpr(acesso->filho2, out);

    int tipoVetorEhCar = (entrada->tipo == TIPO_VETOR_CAR);
    if (!tipoVetorEhCar) {
        /* elemento de 4 bytes: endereco = base + indice*4 (indice << 2) */
        fprintf(out, "    sll   $t%d, $t%d, 2          # indice * 4 (elemento int)\n",
                regIndice, regIndice);
    }
    /* elemento de 1 byte (car): endereco = base + indice, sem escala */

    liberarReg(); /* regIndice */
    liberarReg(); /* regBase   */
    int regEndereco = alocarReg();
    fprintf(out, "    add   $t%d, $t%d, $t%d         # endereco do elemento\n",
            regEndereco, regBase, regIndice);

    *tipoElemento = tipoVetorEhCar ? TIPO_CAR : TIPO_INT;
    return regEndereco;
}

/* =====================================================================
 * Geração de expressões
 *
 * Cada caso devolve o número do registrador temporário onde o
 * resultado da expressão ficou. Para vetores, ver gerarEnderecoVetor.
 * ===================================================================== */

static int gerarExpr(No *no, FILE *out) {
    if (!no) return -1;

    int reg, regEsq, regDir;

    switch (no->tipo) {

        case AST_INTCONST:
            reg = alocarReg();
            fprintf(out, "    li    $t%d, %s\n", reg, no->valor);
            return reg;

        case AST_CARCONST: {
            reg = alocarReg();
            char c = no->valor[1];
            if (c == '\\') {
                char esc = no->valor[2];
                switch (esc) {
                    case 'n': c = '\n'; break;
                    case 't': c = '\t'; break;
                    case '0': c = '\0'; break;
                    default:  c = esc;  break;
                }
            }
            fprintf(out, "    li    $t%d, %d\n", reg, (int)c);
            return reg;
        }

        /* --- Identificador: carrega o valor do slot da variável --- */
        case AST_IDENTIFICADOR: {
            EntradaGerador *entrada = buscarVarGen(no->valor);
            const char *base = entrada->global ? "$s1" : "$fp";
            reg = alocarReg();
            if (entrada->tipo == TIPO_CAR) {
                fprintf(out, "    lb    $t%d, %d(%s)        # %s\n", reg, entrada->offset, base, no->valor);
            } else {
                /* TIPO_INT, TIPO_VETOR_INT ou TIPO_VETOR_CAR: o slot
                   guarda uma word inteira (valor ou ponteiro) */
                fprintf(out, "    lw    $t%d, %d(%s)        # %s\n", reg, entrada->offset, base, no->valor);
            }
            return reg;
        }

        /* --- Acesso a vetor: v[i] --- */
        case AST_VETOR_ACESSO: {
            TipoVar tipoElemento;
            int regEndereco = gerarEnderecoVetor(no, out, &tipoElemento);
            /* le o valor por cima do PROPRIO registrador do endereco —
               depois de ler, o endereco em si nao faz mais falta, e
               reaproveitar o registrador evita alocar mais um (e evita
               o bug de "liberar demais" que isso causaria, deixando o
               resultado fora do controle do contador) */
            if (tipoElemento == TIPO_CAR) {
                fprintf(out, "    lb    $t%d, 0($t%d)\n", regEndereco, regEndereco);
            } else {
                fprintf(out, "    lw    $t%d, 0($t%d)\n", regEndereco, regEndereco);
            }
            return regEndereco;
        }

        /*
         * --- Atribuição ---
         * Quatro formas possíveis, todas distinguidas pelo formato do
         * lado esquerdo (lvalue):
         *   (a) escalar = expr             → grava no slot do escalar
         *   (b) v[i]    = expr             → grava no endereço do elemento
         *   (c) vetorY  = vetorX           → copia o PONTEIRO (slot a slot)
         */
        case AST_ATRIB: {
            No *lado = no->filho1;

            if (lado->tipo == AST_VETOR_ACESSO) {
                /* (b) v[i] = expr — avalia o lado DIREITO primeiro, para
                   que o endereco (calculado depois) fique no topo da
                   pilha de registradores; assim basta liberar uma vez
                   (o endereco) e o valor continua intacto e rastreado
                   corretamente, sem precisar realocar nem copiar nada */
                TipoVar tipoElemento;
                int regValor = gerarExpr(no->filho2, out);
                int regEndereco = gerarEnderecoVetor(lado, out, &tipoElemento);
                if (tipoElemento == TIPO_CAR) {
                    fprintf(out, "    sb    $t%d, 0($t%d)\n", regValor, regEndereco);
                } else {
                    fprintf(out, "    sw    $t%d, 0($t%d)\n", regValor, regEndereco);
                }
                liberarReg(); /* libera so o endereco (topo) */
                return regValor;
            }

            /* lado->tipo == AST_IDENTIFICADOR a partir daqui */
            EntradaGerador *entrada = buscarVarGen(lado->valor);
            const char *base = entrada->global ? "$s1" : "$fp";

            if (entrada->tipo == TIPO_VETOR_INT || entrada->tipo == TIPO_VETOR_CAR) {
                /* (c) vetorY = vetorX: copia o ponteiro, slot a slot */
                regDir = gerarExpr(no->filho2, out); /* já é o ponteiro de vetorX */
                fprintf(out, "    sw    $t%d, %d(%s)        # %s = vetor (copia de endereco)\n",
                        regDir, entrada->offset, base, lado->valor);
                return regDir;
            }

            /* (a) escalar = expr */
            regDir = gerarExpr(no->filho2, out);
            if (entrada->tipo == TIPO_CAR) {
                fprintf(out, "    sb    $t%d, %d(%s)        # %s\n", regDir, entrada->offset, base, lado->valor);
            } else {
                fprintf(out, "    sw    $t%d, %d(%s)        # %s\n", regDir, entrada->offset, base, lado->valor);
            }
            return regDir;
        }

        /* --- Operadores aritméticos binários --- */
        case AST_SOMA:
        case AST_SUB:
        case AST_MUL:
        case AST_DIV: {
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            liberarReg(); liberarReg(); /* libera ANTES de alocar o resultado */
            reg = alocarReg();
            switch (no->tipo) {
                case AST_SOMA:
                    fprintf(out, "    add   $t%d, $t%d, $t%d\n", reg, regEsq, regDir);
                    break;
                case AST_SUB:
                    fprintf(out, "    sub   $t%d, $t%d, $t%d\n", reg, regEsq, regDir);
                    break;
                case AST_MUL:
                    fprintf(out, "    mul   $t%d, $t%d, $t%d\n", reg, regEsq, regDir);
                    break;
                case AST_DIV:
                    fprintf(out, "    div   $t%d, $t%d\n", regEsq, regDir);
                    fprintf(out, "    mflo  $t%d\n", reg);
                    break;
                default: break;
            }
            return reg;
        }

        /* --- Unários: negação aritmética e lógica --- */
        case AST_NEG:
            regEsq = gerarExpr(no->filho1, out);
            liberarReg();
            reg = alocarReg();
            fprintf(out, "    sub   $t%d, $zero, $t%d\n", reg, regEsq);
            return reg;

        case AST_NAO:
            regEsq = gerarExpr(no->filho1, out);
            liberarReg();
            reg = alocarReg();
            fprintf(out, "    seq   $t%d, $t%d, $zero\n", reg, regEsq);
            return reg;

        /* --- Relacionais --- */
        case AST_MENOR:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            liberarReg(); liberarReg();
            reg = alocarReg();
            fprintf(out, "    slt   $t%d, $t%d, $t%d\n", reg, regEsq, regDir);
            return reg;

        case AST_MAIOR:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            liberarReg(); liberarReg();
            reg = alocarReg();
            fprintf(out, "    slt   $t%d, $t%d, $t%d\n", reg, regDir, regEsq); /* a>b ≡ b<a */
            return reg;

        case AST_MENORIGUAL:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            liberarReg(); liberarReg();
            reg = alocarReg();
            fprintf(out, "    slt   $t%d, $t%d, $t%d\n", reg, regDir, regEsq); /* a<=b ≡ !(b<a) */
            fprintf(out, "    xori  $t%d, $t%d, 1\n", reg, reg);
            return reg;

        case AST_MAIORIGUAL:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            liberarReg(); liberarReg();
            reg = alocarReg();
            fprintf(out, "    slt   $t%d, $t%d, $t%d\n", reg, regEsq, regDir); /* a>=b ≡ !(a<b) */
            fprintf(out, "    xori  $t%d, $t%d, 1\n", reg, reg);
            return reg;

        case AST_IGUAL:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            liberarReg(); liberarReg();
            reg = alocarReg();
            fprintf(out, "    seq   $t%d, $t%d, $t%d\n", reg, regEsq, regDir);
            return reg;

        case AST_DIFERENTE:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            liberarReg(); liberarReg();
            reg = alocarReg();
            fprintf(out, "    sne   $t%d, $t%d, $t%d\n", reg, regEsq, regDir);
            return reg;

        /* --- Lógicos --- */
        case AST_E:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            liberarReg(); liberarReg();
            reg = alocarReg();
            fprintf(out, "    and   $t%d, $t%d, $t%d\n", reg, regEsq, regDir);
            return reg;

        case AST_OU:
            regEsq = gerarExpr(no->filho1, out);
            regDir = gerarExpr(no->filho2, out);
            liberarReg(); liberarReg();
            reg = alocarReg();
            fprintf(out, "    or    $t%d, $t%d, $t%d\n", reg, regEsq, regDir);
            return reg;

        /*
         * --- Chamada de função ---
         *
         * Convenção (ver gerador.h): empilha os argumentos do ÚLTIMO
         * para o PRIMEIRO, salta com jal, e recolhe o retorno em $v0.
         * Como $t0-$t9 NÃO sobrevivem a uma chamada, qualquer
         * temporário do CHAMADOR que já esteja "vivo" no meio de uma
         * expressão (ex: o "1" em "1 + f(2)") precisa ser salvo na
         * pilha antes do jal e restaurado depois.
         */
        case AST_CHAMADA_FUNC: {
            int regsVivos = regsUsados; /* temporarios do chamador que precisam sobreviver */
            int i;

            for (i = 0; i < regsVivos; i++) {
                fprintf(out, "    addi  $sp, $sp, -4\n");
                fprintf(out, "    sw    $t%d, 0($sp)        # salva temporario vivo antes da chamada\n", i);
            }

            /* coleta os argumentos num vetor C, na ordem em que aparecem
               (ver coletarArgsChamada, mesma logica de semantico.c para
               a lista esquerda-recursiva do bison) e empilha do ultimo
               para o primeiro */
            No *argsNo[64];
            int numArgs = no->filho2 ? coletarArgsChamada(no->filho2, argsNo) : 0;

            for (i = numArgs - 1; i >= 0; i--) {
                int regArg;
                /* se o parametro correspondente e vetor, o argumento e
                   um identificador de vetor: gerarExpr ja devolve o
                   PONTEIRO guardado no slot (ver case AST_IDENTIFICADOR),
                   exatamente o que precisamos empilhar */
                regArg = gerarExpr(argsNo[i], out);
                fprintf(out, "    addi  $sp, $sp, -4\n");
                fprintf(out, "    sw    $t%d, 0($sp)        # argumento %d\n", regArg, i + 1);
                liberarReg();
            }

            fprintf(out, "    jal   _func_%s\n", no->filho1->valor);

            for (i = regsVivos - 1; i >= 0; i--) {
                fprintf(out, "    lw    $t%d, 0($sp)        # restaura temporario vivo apos a chamada\n", i);
                fprintf(out, "    addi  $sp, $sp, 4\n");
            }

            reg = alocarReg();
            fprintf(out, "    move  $t%d, $v0          # resultado de '%s'\n", reg, no->filho1->valor);
            return reg;
        }

        default:
            return -1;
    }
}

/*
 * coletarArgsChamada — achata a lista de argumentos de uma chamada
 * (AST_LISTA_EXPR, construída esquerda-recursiva pelo bison) num vetor
 * C, na ordem de escrita. Mesma lógica usada em semantico.c.
 */
static int coletarArgsChamada(No *lista, No **out) {
    if (!lista) return 0;
    if (lista->filho2 == NULL) {
        out[0] = lista->filho1;
        return 1;
    }
    int n = coletarArgsChamada(lista->filho1, out);
    out[n] = lista->filho2;
    return n + 1;
}

/* =====================================================================
 * Declaração de variáveis e vetores (locais e globais)
 *
 * Para cada variável: reserva um "slot" de 4 bytes (valor, se escalar;
 * ponteiro, se vetor). Para vetores, reserva TAMBÉM uma área de dados
 * separada (do tamanho do vetor) e, ao final, emite o código que
 * calcula o endereço dessa área e o guarda no slot.
 *
 * Os offsets dos slots ficam conhecidos imediatamente; os offsets das
 * áreas de dados só podem ser calculados depois de saber quantos slots
 * existem no total — por isso o processamento é em duas fases.
 * ===================================================================== */

typedef struct PendenteVetor {
    EntradaGerador *entrada;
    int tamanhoBytes; /* tamanho da area de dados deste vetor */
    struct PendenteVetor *prox;
} PendenteVetor;

/*
 * processarDeclaracoesGen — fase 1: percorre a AST_LISTA_DECL_VAR
 * (mesma estrutura de semantico.c, incluindo a particularidade de que
 * só a CABEÇA de uma linha "x, y, z: int" carrega o nó de Tipo) e:
 *   - para cada nome, calcula o offset do slot (positivo e crescente
 *     para globais; negativo e decrescente para locais) e insere na
 *     tabela correspondente;
 *   - para cada vetor, acumula um nó "pendente" com o tamanho da área
 *     de dados, para a fase 2 calcular o offset dela.
 *
 * 'ehGlobal' decide a tabela (inserirGlobalGen vs inserirVarGen) e o
 * sentido dos offsets dos slots. 'offsetSlot' é passado por referência
 * e atualizado a cada variável inserida.
 */
static void inserirDeclChainGen(No *decl, TipoVar tipoBase, int ehGlobal,
                                 int *offsetSlot, PendenteVetor **pendentes) {
    if (!decl) return;

    No *alvo = decl->filho1; /* AST_IDENTIFICADOR ou AST_VETOR_DECL */
    char *nome;
    TipoVar tipoFinal;
    int ehVetor = (alvo->tipo == AST_VETOR_DECL);

    if (ehVetor) {
        nome = alvo->filho1->valor;
        tipoFinal = (tipoBase == TIPO_INT) ? TIPO_VETOR_INT : TIPO_VETOR_CAR;
    } else {
        nome = alvo->valor;
        tipoFinal = tipoBase;
    }

    int offsetEsteSlot;
    if (ehGlobal) {
        offsetEsteSlot = *offsetSlot;
        *offsetSlot += 4;
        inserirGlobalGen(nome, tipoFinal, offsetEsteSlot);
    } else {
        *offsetSlot -= 4;
        offsetEsteSlot = *offsetSlot;
        inserirVarGen(nome, tipoFinal, offsetEsteSlot);
    }

    if (ehVetor) {
        int tamanhoElemento = (tipoBase == TIPO_CAR) ? 1 : 4;
        int dimensao = atoi(alvo->filho2->valor);
        int tamanhoBruto = tamanhoElemento * dimensao;
        /* arredonda para multiplo de 4: um vetor de car (1 byte por
           elemento) pode terminar num endereco "quebrado"; se o
           PROXIMO vetor na area de dados for de int, lw/sw exigem
           endereco multiplo de 4 — arredondar aqui garante que toda
           area de dados comece alinhada, nao só a deste vetor */
        int tamanhoAlinhado = ((tamanhoBruto + 3) / 4) * 4;

        PendenteVetor *p = (PendenteVetor *)malloc(sizeof(PendenteVetor));
        p->entrada = ehGlobal ? tabelaGlobaisGen : pilhaGen->topo->entradas;
        p->tamanhoBytes = tamanhoAlinhado;
        p->prox = *pendentes;
        *pendentes = p;
    }

    inserirDeclChainGen(decl->filho3, tipoBase, ehGlobal, offsetSlot, pendentes);
}

static void processarDeclaracoesGen(No *lista, int ehGlobal, int *offsetSlot,
                                     PendenteVetor **pendentes) {
    if (!lista) return;

    No *decl = lista->filho1;
    TipoVar tipoBase = (decl->filho2->tipo == AST_TIPO_INT) ? TIPO_INT : TIPO_CAR;
    inserirDeclChainGen(decl, tipoBase, ehGlobal, offsetSlot, pendentes);

    processarDeclaracoesGen(lista->filho2, ehGlobal, offsetSlot, pendentes);
}

/*
 * gerarDeclaracoesLocais — processa a seção de declarações de um bloco
 * (função ou bloco aninhado), faz as duas fases, EMITE o código que
 * reserva espaço na pilha e inicializa os ponteiros dos vetores, e
 * devolve o total de bytes reservados (para o chamador desfazer ao
 * sair do bloco).
 */
static int gerarDeclaracoesLocais(No *lista, FILE *out) {
    int offsetSlot = pilhaGen->offsetAtual;
    PendenteVetor *pendentes = NULL;

    processarDeclaracoesGen(lista, 0, &offsetSlot, &pendentes);

    int slotsBytes = pilhaGen->offsetAtual - offsetSlot; /* quanto cresceu */
    pilhaGen->offsetAtual = offsetSlot;

    int cursorDados = offsetSlot; /* area de dados comeca logo apos os slots */
    int totalArraysBytes = 0;
    PendenteVetor *p = pendentes;
    while (p) {
        cursorDados -= p->tamanhoBytes;
        totalArraysBytes += p->tamanhoBytes;

        int r = alocarReg();
        fprintf(out, "    addi  $t%d, $fp, %d        # endereco dos dados do vetor '%s'\n",
                r, cursorDados, p->entrada->nome);
        fprintf(out, "    sw    $t%d, %d($fp)        # guarda no slot (ponteiro)\n",
                r, p->entrada->offset);
        liberarReg();

        PendenteVetor *prox = p->prox;
        free(p);
        p = prox;
    }
    pilhaGen->offsetAtual = cursorDados;

    int total = slotsBytes + totalArraysBytes;
    pilhaGen->topo->tamanho += total;
    return total;
}

/*
 * gerarDeclaracoesGlobais — só a fase 1+2 de BOOKKEEPING das variáveis
 * globais (sem emitir nenhuma instrução: ainda não sabemos o endereço
 * de '_globais' nem temos o arquivo de saída aberto nesse ponto — ver
 * gerarCodigo). Devolve o total de bytes que a seção '.data' precisa.
 *
 * Os offsets das áreas de dados dos vetores ficam guardados numa lista
 * separada (globaisPendentes) para inicializarVetoresGlobais emitir o
 * código de inicialização depois, já com o arquivo de saída disponível.
 */
static PendenteVetor *globaisPendentes = NULL;
static int slotsBytesGlobaisGen = 0; /* tamanho so dos slots (sem a area de dados dos vetores) */

static int gerarDeclaracoesGlobais(No *lista) {
    int offsetSlot = 0;
    processarDeclaracoesGen(lista, 1, &offsetSlot, &globaisPendentes);

    int slotsBytes = offsetSlot;
    slotsBytesGlobaisGen = slotsBytes;

    int totalArraysBytes = 0;
    PendenteVetor *p = globaisPendentes;
    while (p) {
        totalArraysBytes += p->tamanhoBytes;
        p = p->prox;
    }

    return slotsBytes + totalArraysBytes;
}

/*
 * inicializarVetoresGlobais — emite, para cada vetor global, o código
 * que calcula o endereço da sua área de dados (a partir de $s1) e o
 * guarda no slot. Chamada uma única vez, no prólogo de "main".
 */
static void inicializarVetoresGlobais(FILE *out, int slotsBytesGlobais) {
    int cursorDados = slotsBytesGlobais;
    PendenteVetor *p = globaisPendentes;
    while (p) {
        int r = alocarReg();
        fprintf(out, "    addi  $t%d, $s1, %d        # endereco dos dados do vetor global '%s'\n",
                r, cursorDados, p->entrada->nome);
        fprintf(out, "    sw    $t%d, %d($s1)        # guarda no slot (ponteiro)\n",
                r, p->entrada->offset);
        liberarReg();

        cursorDados += p->tamanhoBytes;
        p = p->prox;
    }
}

/* =====================================================================
 * Geração de comandos
 * ===================================================================== */

/*
 * Estado da "função atual", usado por AST_CMD_RETORNE: para onde saltar
 * (epílogo) e quantos bytes desfazer (locais do bloco mais externo +
 * argumentos empilhados pelo chamador).
 */
static int labelFimFuncaoAtual = -1;
static int bytesLocaisFuncaoAtual = 0;
static int bytesArgsFuncaoAtual = 0;
static int dentroDeFuncaoGen = 0;

static void gerarComando(No *no, FILE *out) {
    if (!no) return;

    switch (no->tipo) {

        case AST_LISTA_COMANDO:
            gerarComando(no->filho1, out);
            gerarComando(no->filho2, out);
            break;

        case AST_CMD_VAZIO:
            break;

        case AST_CMD_EXPR: {
            int reg = gerarExpr(no->filho1, out);
            if (reg >= 0) liberarReg();
            break;
        }

        /* leia x; ou leia v[i]; */
        case AST_CMD_LEIA: {
            No *destino = no->filho1;

            if (destino->tipo == AST_VETOR_ACESSO) {
                TipoVar tipoElemento;
                int regEndereco = gerarEnderecoVetor(destino, out, &tipoElemento);
                if (tipoElemento == TIPO_CAR) {
                    fprintf(out, "    li    $v0, 12\n    syscall\n");
                    fprintf(out, "    sb    $v0, 0($t%d)\n", regEndereco);
                } else {
                    fprintf(out, "    li    $v0, 5\n    syscall\n");
                    fprintf(out, "    sw    $v0, 0($t%d)\n", regEndereco);
                }
                liberarReg();
            } else {
                EntradaGerador *entrada = buscarVarGen(destino->valor);
                const char *base = entrada->global ? "$s1" : "$fp";
                if (entrada->tipo == TIPO_CAR) {
                    fprintf(out, "    li    $v0, 12       # leia car: %s\n    syscall\n", destino->valor);
                    fprintf(out, "    sb    $v0, %d(%s)\n", entrada->offset, base);
                } else {
                    fprintf(out, "    li    $v0, 5        # leia int: %s\n    syscall\n", destino->valor);
                    fprintf(out, "    sw    $v0, %d(%s)\n", entrada->offset, base);
                }
            }
            break;
        }

        /* escreva expr; */
        case AST_CMD_ESCREVA_EXPR: {
            TipoVar tipo = tipoExpressao(no->filho1);
            int reg = gerarExpr(no->filho1, out);
            int syscallNum = (tipo == TIPO_CAR) ? 11 : 1;
            fprintf(out, "    move  $a0, $t%d\n", reg);
            fprintf(out, "    li    $v0, %d\n", syscallNum);
            fprintf(out, "    syscall\n");
            liberarReg();
            break;
        }

        /* escreva "string"; */
        case AST_CMD_ESCREVA_STR: {
            int id = novoLabel();
            fprintf(out, "    la    $a0, _str%d\n", id);
            fprintf(out, "    li    $v0, 4\n");
            fprintf(out, "    syscall\n");
            fprintf(out, "    .data\n");
            fprintf(out, "_str%d: .asciiz %s\n", id, no->valor);
            fprintf(out, "    .text\n");
            break;
        }

        case AST_CMD_NOVALINHA:
            fprintf(out, "    li    $a0, 10\n");
            fprintf(out, "    li    $v0, 11\n");
            fprintf(out, "    syscall\n");
            break;

        case AST_CMD_SE: {
            int labelFim = novoLabel();
            int regCond = gerarExpr(no->filho1, out);
            fprintf(out, "    beqz  $t%d, _label%d\n", regCond, labelFim);
            liberarReg();
            gerarComando(no->filho2, out);
            fprintf(out, "_label%d:\n", labelFim);
            break;
        }

        case AST_CMD_SE_SENAO: {
            int labelSenao = novoLabel();
            int labelFim   = novoLabel();
            int regCond = gerarExpr(no->filho1, out);
            fprintf(out, "    beqz  $t%d, _label%d\n", regCond, labelSenao);
            liberarReg();
            gerarComando(no->filho2, out);
            fprintf(out, "    j     _label%d\n", labelFim);
            fprintf(out, "_label%d:\n", labelSenao);
            gerarComando(no->filho3, out);
            fprintf(out, "_label%d:\n", labelFim);
            break;
        }

        case AST_CMD_ENQUANTO: {
            int labelInicio = novoLabel();
            int labelFim    = novoLabel();
            fprintf(out, "_label%d:\n", labelInicio);
            int regCond = gerarExpr(no->filho1, out);
            fprintf(out, "    beqz  $t%d, _label%d\n", regCond, labelFim);
            liberarReg();
            gerarComando(no->filho2, out);
            fprintf(out, "    j     _label%d\n", labelInicio);
            fprintf(out, "_label%d:\n", labelFim);
            break;
        }

        /*
         * retorne expr; — avalia a expressão, guarda em $v0 e salta
         * direto para o epílogo da função atual (que desfaz o frame e
         * devolve para o chamador). Não há "fim de função" separado:
         * todo caminho de retorno passa por aqui.
         */
        case AST_CMD_RETORNE: {
            int reg = gerarExpr(no->filho1, out);
            fprintf(out, "    move  $v0, $t%d          # valor de retorno\n", reg);
            liberarReg();
            fprintf(out, "    j     _fim_func%d\n", labelFimFuncaoAtual);
            break;
        }

        case AST_BLOCO:
            gerarBloco(no, out);
            break;

        default:
            break;
    }
}

/*
 * gerarBloco — bloco aninhado dentro de comandos (se/enquanto/bloco
 * solto). NÃO usado para o bloco mais externo de uma função — esse é
 * tratado especialmente em gerarFuncao (ver comentário lá).
 */
static void gerarBloco(No *no, FILE *out) {
    if (!no || no->tipo != AST_BLOCO) return;

    empilharEscopoGen();
    int bytes = gerarDeclaracoesLocais(no->filho1, out);

    if (bytes > 0) {
        fprintf(out, "    addi  $sp, $sp, -%d        # abre escopo (vars/vetores locais)\n", bytes);
    }

    gerarComando(no->filho2, out);

    int liberado = removerEscopoGen();
    if (liberado > 0) {
        fprintf(out, "    addi  $sp, $sp, %d         # fecha escopo\n", liberado);
    }
}

/* =====================================================================
 * Funções
 * ===================================================================== */

/*
 * registrarFuncoes — primeira passada sobre a lista de funções: só
 * registra nome + tipo de retorno na tabela de funções (necessário
 * para tipoExpressao funcionar mesmo quando uma função chama outra
 * declarada depois dela na lista).
 */
static void registrarFuncoes(No *listaFunc) {
    if (!listaFunc) return;
    No *func = listaFunc->filho1;
    TipoVar tipoRetorno = (func->filho2->tipo == AST_TIPO_INT) ? TIPO_INT : TIPO_CAR;
    inserirFuncaoGen(func->filho1->valor, tipoRetorno);
    registrarFuncoes(listaFunc->filho2);
}

/*
 * gerarFuncao — gera o código completo de UMA função: label, prólogo
 * (salva $ra/$fp antigo, monta o novo frame, reserva locais), corpo, e
 * epílogo (desfaz tudo, inclusive os argumentos empilhados pelo
 * chamador, e retorna com jr $ra).
 *
 * Parâmetros e variáveis do bloco mais externo entram no MESMO escopo
 * do gerador — não porque isso afete o código gerado (cada um já tem
 * seu próprio offset, positivo ou negativo), mas para manter a mesma
 * organização conceitual usada na análise semântica.
 */
static void gerarFuncao(No *func, FILE *out) {
    char *nome = func->filho1->valor;

    /* conta os parametros para saber quantos bytes o chamador empilhou */
    int numParametros = 0;
    No *p = func->filho3;
    while (p) { numParametros++; p = p->filho2; }
    int bytesArgs = 4 * numParametros;

    int labelFim = novoLabel();
    int labelFimAnterior   = labelFimFuncaoAtual;
    int bytesArgsAnterior  = bytesArgsFuncaoAtual;
    int dentroAnterior     = dentroDeFuncaoGen;
    labelFimFuncaoAtual  = labelFim;
    bytesArgsFuncaoAtual = bytesArgs;
    dentroDeFuncaoGen    = 1;

    fprintf(out, "\n_func_%s:\n", nome);
    fprintf(out, "    addi  $sp, $sp, -4\n");
    fprintf(out, "    sw    $ra, 0($sp)         # salva endereco de retorno\n");
    fprintf(out, "    addi  $sp, $sp, -4\n");
    fprintf(out, "    sw    $fp, 0($sp)         # salva fp do chamador\n");
    fprintf(out, "    move  $fp, $sp            # novo frame comeca aqui\n");

    iniciarPilhaGen();
    empilharEscopoGen();

    /* parametros: offset(p) = 8 + 4*(p-1), p = posicao 1..N (ver gerador.h) */
    int posicao = 1;
    p = func->filho3;
    while (p) {
        No *param = p->filho1;
        No *alvo = param->filho1;
        TipoVar base = (param->filho2->tipo == AST_TIPO_INT) ? TIPO_INT : TIPO_CAR;
        char *nomeParam;
        TipoVar tipoParam;
        if (alvo->tipo == AST_VETOR_DECL) {
            nomeParam = alvo->filho1->valor;
            tipoParam = (base == TIPO_INT) ? TIPO_VETOR_INT : TIPO_VETOR_CAR;
        } else {
            nomeParam = alvo->valor;
            tipoParam = base;
        }
        inserirVarGen(nomeParam, tipoParam, 8 + 4 * (posicao - 1));
        posicao++;
        p = p->filho2;
    }

    /* bloco mais externo da funcao: declaracoes locais + comandos */
    No *bloco = func->filho4;
    int bytesLocais = gerarDeclaracoesLocais(bloco->filho1, out);
    bytesLocaisFuncaoAtual = bytesLocais;

    if (bytesLocais > 0) {
        fprintf(out, "    addi  $sp, $sp, -%d        # reserva vars/vetores locais\n", bytesLocais);
    }

    gerarComando(bloco->filho2, out);

    /* epilogo: todo "retorne" salta para aqui (ver AST_CMD_RETORNE) */
    fprintf(out, "_fim_func%d:\n", labelFim);
    if (bytesLocais > 0) {
        fprintf(out, "    addi  $sp, $sp, %d         # libera vars/vetores locais\n", bytesLocais);
    }
    removerEscopoGen();
    free(pilhaGen);
    pilhaGen = NULL;

    fprintf(out, "    lw    $fp, 0($sp)         # restaura fp do chamador\n");
    fprintf(out, "    addi  $sp, $sp, 4\n");
    fprintf(out, "    lw    $ra, 0($sp)         # restaura endereco de retorno\n");
    fprintf(out, "    addi  $sp, $sp, 4\n");
    if (bytesArgs > 0) {
        fprintf(out, "    addi  $sp, $sp, %d         # remove argumentos empilhados pelo chamador\n", bytesArgs);
    }
    fprintf(out, "    jr    $ra\n");

    labelFimFuncaoAtual  = labelFimAnterior;
    bytesArgsFuncaoAtual = bytesArgsAnterior;
    dentroDeFuncaoGen    = dentroAnterior;
}

static void gerarTodasFuncoes(No *listaFunc, FILE *out) {
    if (!listaFunc) return;
    gerarFuncao(listaFunc->filho1, out);
    gerarTodasFuncoes(listaFunc->filho2, out);
}

/* =====================================================================
 * Ponto de entrada da geração de código
 * ===================================================================== */

void gerarCodigo(No *raiz, FILE *saida) {
    if (!raiz || raiz->tipo != AST_PROGRAMA) return;

    /*
     * Passo 1 — registra as variáveis globais (só bookkeeping: ainda
     * não sabemos o arquivo de saída nem podemos emitir nada, porque
     * o tamanho da seção '.data' só fica conhecido depois desta conta).
     */
    int bytesGlobais = 0;
    if (raiz->filho1) { /* AST_GLOBAIS */
        bytesGlobais = gerarDeclaracoesGlobais(raiz->filho1->filho1);
    }

    /* Passo 2 — cabeçalho do arquivo assembly */
    fprintf(saida, "# Codigo MIPS gerado pelo compilador G-V2\n");
    if (bytesGlobais > 0) {
        fprintf(saida, "    .data\n");
        fprintf(saida, "_globais: .space %d\n", bytesGlobais);
    }
    fprintf(saida, "    .text\n");
    fprintf(saida, "    .globl main\n");
    fprintf(saida, "main:\n");

    if (bytesGlobais > 0) {
        fprintf(saida, "    la    $s1, _globais       # base fixa das variaveis globais\n");
        inicializarVetoresGlobais(saida, slotsBytesGlobaisGen);
    }

    /*
     * Passo 3 — registra os cabeçalhos de TODAS as funções (nome +
     * tipo de retorno) antes de gerar qualquer corpo, do mesmo jeito
     * que a análise semântica faz, para suportar chamada para frente e
     * recursão mútua.
     */
    registrarFuncoes(raiz->filho2);

    /*
     * Passo 4 — bloco "principal": funciona como um bloco comum, só
     * que sem prólogo/epílogo de chamada (não há "retorne" nem
     * chamador a quem voltar — o programa termina com a syscall 10).
     */
    fprintf(saida, "    move  $fp, $sp            # fp do bloco principal\n");
    iniciarPilhaGen();
    gerarBloco(raiz->filho3, saida);
    free(pilhaGen);
    pilhaGen = NULL;

    fprintf(saida, "    li    $v0, 10             # encerra o programa\n");
    fprintf(saida, "    syscall\n");

    /*
     * Passo 5 — gera o código de cada função, depois do "main" (a
     * ordem no arquivo .s não importa: jal/labels resolvem isso).
     */
    gerarTodasFuncoes(raiz->filho2, saida);
}
