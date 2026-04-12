/*
 * ast.h — Definição da Árvore Sintática Abstrata (AST)
 *
 * A AST é a estrutura de dados central do compilador. Após a análise
 * sintática, o programa de entrada é representado em memória como uma
 * árvore, onde cada nó corresponde a uma construção da linguagem G-V1
 * (bloco, comando, expressão, declaração, etc.).
 *
 * As fases seguintes (análise semântica e geração de código) percorrem
 * essa árvore para realizar seu trabalho.
 */

#ifndef AST_H
#define AST_H

/*
 * TipoNo — Enumeração com todos os tipos de nós possíveis na AST.
 *
 * Cada valor representa uma construção diferente da linguagem G-V1.
 * O tipo do nó determina quantos filhos ele tem e o que eles significam.
 */
typedef enum {

    /* --- Estrutura do programa --- */
    AST_PROGRAMA,       /* raiz da árvore; filho1 = bloco principal        */
    AST_BLOCO,          /* filho1 = declarações (ou NULL), filho2 = cmds   */

    /* --- Declarações de variáveis --- */
    AST_LISTA_DECL_VAR, /* filho1 = primeira decl, filho2 = próxima lista  */
    AST_DECL_VAR,       /* filho1 = IDENTIFICADOR, filho2 = tipo,
                           filho3 = próximas vars na mesma linha (vírgula) */

    /* --- Tipos --- */
    AST_TIPO_INT,       /* palavra-chave 'int'                              */
    AST_TIPO_CAR,       /* palavra-chave 'car'                              */

    /* --- Lista de comandos --- */
    AST_LISTA_COMANDO,  /* filho1 = comando atual, filho2 = próxima lista   */

    /* --- Comandos --- */
    AST_CMD_VAZIO,          /* ';' — comando vazio                          */
    AST_CMD_EXPR,           /* expressão seguida de ';'; filho1 = expr      */
    AST_CMD_LEIA,           /* leia id; filho1 = IDENTIFICADOR              */
    AST_CMD_ESCREVA_EXPR,   /* escreva expr; filho1 = expr                  */
    AST_CMD_ESCREVA_STR,    /* escreva "string"; valor = texto da string    */
    AST_CMD_NOVALINHA,      /* novalinha;                                   */
    AST_CMD_SE,             /* se(expr) entao cmd fimse
                               filho1 = condição, filho2 = cmd then        */
    AST_CMD_SE_SENAO,       /* se(expr) entao cmd senao cmd fimse
                               filho1 = cond, filho2 = then, filho3 = else */
    AST_CMD_ENQUANTO,       /* enquanto(expr) cmd
                               filho1 = condição, filho2 = corpo           */

    /* --- Expressões --- */
    AST_ATRIB,          /* id = expr; filho1 = id, filho2 = expr           */
    AST_OU,             /* expr || expr                                     */
    AST_E,              /* expr & expr                                      */
    AST_IGUAL,          /* expr == expr                                     */
    AST_DIFERENTE,      /* expr != expr                                     */
    AST_MENOR,          /* expr < expr                                      */
    AST_MAIOR,          /* expr > expr                                      */
    AST_MAIORIGUAL,     /* expr >= expr                                     */
    AST_MENORIGUAL,     /* expr <= expr                                     */
    AST_SOMA,           /* expr + expr                                      */
    AST_SUB,            /* expr - expr                                      */
    AST_MUL,            /* expr * expr                                      */
    AST_DIV,            /* expr / expr                                      */
    AST_NEG,            /* -expr (unário); filho1 = expr                   */
    AST_NAO,            /* !expr (unário); filho1 = expr                   */

    /* --- Folhas (nós sem filhos, carregam valor léxico) --- */
    AST_IDENTIFICADOR,      /* nome de variável; valor = lexema             */
    AST_CARCONST,           /* constante de caractere, ex: 'a'             */
    AST_INTCONST,           /* constante inteira, ex: 42                   */
    AST_CADEIACARACTERES    /* string literal, ex: "oi"                    */

} TipoNo;

/*
 * No — Estrutura de um nó da AST.
 *
 * Todos os nós têm o mesmo formato genérico. O campo 'tipo' indica o que
 * o nó representa. Os filhos apontam para sub-árvores (NULL se não existir).
 * Nós folha não têm filhos, mas carregam o texto léxico em 'valor'.
 */
typedef struct No {
    TipoNo tipo;        /* que construção da linguagem este nó representa   */
    int linha;          /* linha no programa fonte (para mensagens de erro) */
    char *valor;        /* texto léxico — usado apenas em nós folha         */

    struct No *filho1;  /* primeiro filho (significado depende do tipo)     */
    struct No *filho2;  /* segundo filho                                    */
    struct No *filho3;  /* terceiro filho (usado em se-senao e decl-var)   */
} No;

/* --- Funções de criação de nós --- */

/* Cria um nó folha: sem filhos, mas com valor léxico (ex: identificador) */
No *novoNo(TipoNo tipo, int linha, const char *valor);

/* Cria um nó com 1 filho (ex: negação unária, comando de expressão) */
No *novoNo1(TipoNo tipo, int linha, No *filho1);

/* Cria um nó com 2 filhos (ex: soma, atribuição, bloco) */
No *novoNo2(TipoNo tipo, int linha, No *filho1, No *filho2);

/* Cria um nó com 3 filhos (ex: se-senao, decl-var com vírgula) */
No *novoNo3(TipoNo tipo, int linha, No *filho1, No *filho2, No *filho3);

/* Imprime a AST de forma indentada — útil para depuração e apresentação */
void imprimirAST(No *no, int profundidade);

/* Libera toda a memória alocada pela AST recursivamente */
void liberarAST(No *no);

#endif /* AST_H */
