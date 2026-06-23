/*
 * ast.h — Definição da Árvore Sintática Abstrata (AST) para G-V2
 *
 * A AST é a estrutura de dados central do compilador. Após a análise
 * sintática, o programa de entrada é representado em memória como uma
 * árvore, onde cada nó corresponde a uma construção da linguagem G-V2.
 *
 * As fases seguintes (análise semântica e geração de código) percorrem
 * essa árvore para realizar seu trabalho.
 */

#ifndef AST_H
#define AST_H

/*
 * TipoNo — Enumeração com todos os tipos de nós possíveis na AST.
 */
typedef enum {

    /* --- Estrutura do programa --- */
    AST_PROGRAMA,       /* filho1=globais, filho2=funcoes, filho3=bloco_principal */
    AST_BLOCO,          /* filho1 = declarações (ou NULL), filho2 = cmds   */

    /* --- Declarações Globais --- */
    AST_GLOBAIS,        /* filho1 = VarSection (seção global) */

    /* --- Declarações de variáveis --- */
    AST_LISTA_DECL_VAR, /* filho1 = primeira decl, filho2 = próxima lista  */
    AST_DECL_VAR,       /* filho1=ID/VETOR, filho2=tipo, filho3=extras(,)  */

    /* --- Declaração de funções e Parâmetros --- */
    AST_LISTA_FUNC,     /* filho1 = função atual, filho2 = próxima lista   */
    AST_FUNCAO,         /* filho1=ID, filho2=TIPO, filho3=PARAMS, filho4=BLOCO */
    AST_LISTA_PARAM,    /* filho1 = parâmetro atual, filho2 = próxima lista */
    AST_PARAMETRO,      /* filho1 = ID/VETOR, filho2 = TIPO                */

    /* --- Declaração e Acesso a Vetores --- */
    AST_VETOR_DECL,     /* filho1 = ID, filho2 = Expressão de dimensão (ou NULL p/ param) */
    AST_VETOR_ACESSO,   /* filho1 = ID, filho2 = Expressão de índice       */

    /* --- Chamadas de Função --- */
    AST_CHAMADA_FUNC,   /* filho1 = ID, filho2 = lista de expressões (args)*/
    AST_LISTA_EXPR,     /* filho1 = expressão atual, filho2 = próxima expr */

    /* --- Tipos primitivos --- */
    AST_TIPO_INT,       /* palavra-chave 'int'                             */
    AST_TIPO_CAR,       /* palavra-chave 'car'                             */

    /* --- Lista de comandos --- */
    AST_LISTA_COMANDO,  /* filho1 = comando atual, filho2 = próxima lista  */

    /* --- Comandos --- */
    AST_CMD_VAZIO,          /* ';' — comando vazio                         */
    AST_CMD_EXPR,           /* expressão seguida de ';'; filho1 = expr     */
    AST_CMD_LEIA,           /* leia lvalue; filho1 = LValueExpr            */
    AST_CMD_ESCREVA_EXPR,   /* escreva expr; filho1 = expr                 */
    AST_CMD_ESCREVA_STR,    /* escreva "string"; valor = texto da string   */
    AST_CMD_NOVALINHA,      /* novalinha;                                  */
    AST_CMD_SE,             /* se(expr) entao cmd fimse; f1=cond, f2=then  */
    AST_CMD_SE_SENAO,       /* se(expr) entao cmd senao cmd fimse; f1=cond, f2=then, f3=else */
    AST_CMD_ENQUANTO,       /* enquanto(expr) cmd; filho1 = cond, filho2 = corpo */
    AST_CMD_RETORNE,        /* retorne expr; filho1 = expressão a retornar */

    /* --- Expressões --- */
    AST_ATRIB,          /* lvalue = expr; filho1 = lvalue, filho2 = expr   */
    AST_OU,             /* expr || expr                                    */
    AST_E,              /* expr & expr                                     */
    AST_IGUAL,          /* expr == expr                                    */
    AST_DIFERENTE,      /* expr != expr                                    */
    AST_MENOR,          /* expr < expr                                     */
    AST_MAIOR,          /* expr > expr                                     */
    AST_MAIORIGUAL,     /* expr >= expr                                    */
    AST_MENORIGUAL,     /* expr <= expr                                    */
    AST_SOMA,           /* expr + expr                                     */
    AST_SUB,            /* expr - expr                                     */
    AST_MUL,            /* expr * expr                                     */
    AST_DIV,            /* expr / expr                                     */
    AST_NEG,            /* -expr (unário); filho1 = expr                   */
    AST_NAO,            /* !expr (unário); filho1 = expr                   */

    /* --- Folhas (nós sem filhos, carregam valor léxico) --- */
    AST_IDENTIFICADOR,      /* nome de variável ou função; valor = lexema  */
    AST_CARCONST,           /* constante de caractere, ex: 'a'             */
    AST_INTCONST,           /* constante inteira, ex: 42                   */
    AST_CADEIACARACTERES    /* string literal, ex: "oi"                    */

} TipoNo;

/*
 * No — Estrutura de um nó da AST.
 */
typedef struct No {
    TipoNo tipo;        /* construção sintática que este nó representa     */
    int linha;          /* linha no código-fonte (para erros semânticos)   */
    char *valor;        /* texto léxico armazenado (usado em folhas)       */

    struct No *filho1;  /* primeiro filho                                  */
    struct No *filho2;  /* segundo filho                                   */
    struct No *filho3;  /* terceiro filho                                  */
    struct No *filho4;  /* quarto filho (necessário para funções)          */
} No;

/* --- Construtores de Nós --- */

/* Cria um nó folha: sem filhos, mas com valor léxico (ex: identificador) */
No *novoNo(TipoNo tipo, int linha, const char *valor);

/* Cria um nó com 1 filho (ex: retorno, leitura, negação unária) */
No *novoNo1(TipoNo tipo, int linha, No *filho1);

/* Cria um nó com 2 filhos (ex: operadores binários, blocos, declarações) */
No *novoNo2(TipoNo tipo, int linha, No *filho1, No *filho2);

/* Cria um nó com 3 filhos (ex: if-else, declaração composta, programa) */
No *novoNo3(TipoNo tipo, int linha, No *filho1, No *filho2, No *filho3);

/* Cria um nó com 4 filhos (ex: declaração de função) */
No *novoNo4(TipoNo tipo, int linha, No *filho1, No *filho2, No *filho3, No *filho4);

/* --- Funções Utilitárias --- */

/* Imprime a AST no terminal para inspeção visual do parsing */
void imprimirAST(No *no, int profundidade);

/* Libera toda a memória alocada dinamicamente para a árvore */
void liberarAST(No *no);

#endif /* AST_H */