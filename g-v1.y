/*
 * g-v1.y — Especificação do Analisador Sintático (Bison)
 *
 * Este arquivo descreve a gramática da linguagem G-V1 no formato aceito
 * pelo Bison. Além das regras gramaticais, cada produção possui uma
 * "ação semântica" (código C entre chaves) que constrói um nó da AST.
 *
 * O Bison usa o método LALR(1) para gerar o analisador sintático.
 * Ao final da análise bem-sucedida, a variável global 'raizAST' aponta
 * para a raiz da AST construída.
 */

/* ===== SEÇÃO DE DECLARAÇÕES ===== */
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"  /* tipos e funções da AST */
#include "tabela.h" /* tipos e funções da tabela de símbolos */

/* Variáveis externas definidas pelo código gerado pelo Flex */
extern int yylineno;   /* número da linha atual no arquivo de entrada */
extern char *yytext;   /* texto do último token reconhecido            */
extern int yylex();    /* função do analisador léxico gerado pelo Flex */
extern FILE *yyin;     /* ponteiro para o arquivo de entrada           */

/* Raiz da AST — preenchida ao final da análise sintática bem-sucedida */
No *raizAST = NULL;

/* Declaração antecipada da função de erro do Bison */
void yyerror(const char *s);
%}

/*
 * %union — define o tipo semântico dos símbolos da gramática.
 *
 * Tokens com valor léxico (identificadores, constantes) carregam uma
 * string ('str'). Não-terminais carregam um ponteiro para nó da AST ('no').
 */
%union {
    No   *no;   /* ponteiro para nó da AST — usado pelos não-terminais    */
    char *str;  /* texto léxico — usado pelos tokens com valor            */
}

/* Tokens que carregam valor léxico (o lexema reconhecido pelo Flex) */
%token <str> IDENTIFICADOR CADEIACARACTERES CARCONST INTCONST

/* Tokens simples (palavras-reservadas e operadores sem valor associado) */
%token PRINCIPAL INT CAR LEIA ESCREVA NOVALINHA ENQUANTO SE ENTAO SENAO FIMSE
%token MENORIGUAL MAIORIGUAL IGUAL DIFERENTE E OU

/* Tipo semântico dos não-terminais: todos retornam ponteiro para nó da AST */
%type <no> Programa DeclPrograma Bloco VarSection
%type <no> ListaDeclVar DeclVar Tipo
%type <no> ListaComando Comando
%type <no> Expr OrExpr AndExpr EqExpr DesigExpr AddExpr MulExpr UnExpr PrimExpr

/* Símbolo inicial da gramática */
%start Programa

/* ===== SEÇÃO DE REGRAS ===== */
%%

/*
 * Programa — raiz da gramática.
 * Cria o nó raiz da AST englobando toda a estrutura do programa.
 */
Programa
    : DeclPrograma
        { raizAST = novoNo1(AST_PROGRAMA, yylineno, $1); $$ = raizAST; }
    ;

/*
 * DeclPrograma — estrutura principal: "principal Bloco".
 * Retorna diretamente o bloco (a palavra 'principal' não vira nó).
 */
DeclPrograma
    : PRINCIPAL Bloco
        { $$ = $2; }
    ;

/*
 * Bloco — pode ter ou não uma seção de declarações de variáveis.
 *   filho1 = VarSection (declarações) ou NULL se não houver
 *   filho2 = ListaComando (comandos do bloco)
 */
Bloco
    : '{' ListaComando '}'
        { $$ = novoNo2(AST_BLOCO, yylineno, NULL, $2); }
    | VarSection '{' ListaComando '}'
        { $$ = novoNo2(AST_BLOCO, yylineno, $1, $3); }
    ;

/*
 * VarSection — bloco de declarações de variáveis entre chaves.
 * Retorna diretamente a ListaDeclVar.
 */
VarSection
    : '{' ListaDeclVar '}'
        { $$ = $2; }
    ;

/*
 * ListaDeclVar — lista de uma ou mais declarações.
 * Encadeada via filho2: filho1 = declaração atual, filho2 = próxima lista.
 *
 * Exemplo: "x, y : int; z : car;"
 *   → LISTA_DECL_VAR
 *       DECL_VAR (x, int, extras:[y])
 *       LISTA_DECL_VAR
 *         DECL_VAR (z, car)
 */
ListaDeclVar
    : IDENTIFICADOR DeclVar ':' Tipo ';' ListaDeclVar
        {
            /* Nó DECL_VAR: filho1 = nome, filho2 = tipo, filho3 = extras (vírgula) */
            No *decl = novoNo2(AST_DECL_VAR, yylineno,
                               novoNo(AST_IDENTIFICADOR, yylineno, $1), $4);
            decl->filho3 = $2; /* variáveis extras separadas por vírgula */
            $$ = novoNo2(AST_LISTA_DECL_VAR, yylineno, decl, $6);
            free($1); /* libera a string copiada pelo Flex */
        }
    | IDENTIFICADOR DeclVar ':' Tipo ';'
        {
            No *decl = novoNo2(AST_DECL_VAR, yylineno,
                               novoNo(AST_IDENTIFICADOR, yylineno, $1), $4);
            decl->filho3 = $2;
            $$ = novoNo1(AST_LISTA_DECL_VAR, yylineno, decl);
            free($1);
        }
    ;

/*
 * DeclVar — zero ou mais identificadores extras separados por vírgula.
 * Permite declarar várias variáveis do mesmo tipo: "x, y, z : int;"
 * Encadeada recursivamente.
 */
DeclVar
    : /* vazio — não há mais variáveis na linha */
        { $$ = NULL; }
    | ',' IDENTIFICADOR DeclVar
        {
            /* cria nó para este identificador extra e encadeia com o próximo */
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, $2);
            $$ = novoNo2(AST_DECL_VAR, yylineno, id, $3);
            free($2);
        }
    ;

/*
 * Tipo — INT ou CAR.
 * Cria um nó folha indicando o tipo da variável.
 */
Tipo
    : INT  { $$ = novoNo(AST_TIPO_INT, yylineno, NULL); }
    | CAR  { $$ = novoNo(AST_TIPO_CAR, yylineno, NULL); }
    ;

/*
 * ListaComando — sequência de um ou mais comandos.
 * Encadeada via filho2: filho1 = comando atual, filho2 = restante da lista.
 */
ListaComando
    : Comando
        { $$ = novoNo1(AST_LISTA_COMANDO, yylineno, $1); }
    | Comando ListaComando
        { $$ = novoNo2(AST_LISTA_COMANDO, yylineno, $1, $2); }
    ;

/*
 * Comando — todas as formas de comando da linguagem G-V1.
 */
Comando
    /* Comando vazio: apenas ponto-e-vírgula */
    : ';'
        { $$ = novoNo(AST_CMD_VAZIO, yylineno, NULL); }

    /* Expressão como comando: ex. "x = 5;" ou "x + 1;" */
    | Expr ';'
        { $$ = novoNo1(AST_CMD_EXPR, yylineno, $1); }

    /* Leitura de variável: "leia x;" */
    | LEIA IDENTIFICADOR ';'
        {
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, $2);
            $$ = novoNo1(AST_CMD_LEIA, yylineno, id);
            free($2);
        }

    /* Escrita de expressão: "escreva x;" */
    | ESCREVA Expr ';'
        { $$ = novoNo1(AST_CMD_ESCREVA_EXPR, yylineno, $2); }

    /* Escrita de string literal: "escreva "hello";" */
    | ESCREVA CADEIACARACTERES ';'
        {
            /* o valor da string é guardado diretamente no nó (nó folha) */
            $$ = novoNo(AST_CMD_ESCREVA_STR, yylineno, $2);
            free($2);
        }

    /* Quebra de linha: "novalinha;" */
    | NOVALINHA ';'
        { $$ = novoNo(AST_CMD_NOVALINHA, yylineno, NULL); }

    /* Condicional sem senao: "se(expr) entao cmd fimse"
       filho1 = condição, filho2 = comando do then */
    | SE '(' Expr ')' ENTAO Comando FIMSE
        { $$ = novoNo2(AST_CMD_SE, yylineno, $3, $6); }

    /* Condicional com senao: "se(expr) entao cmd senao cmd fimse"
       filho1 = condição, filho2 = then, filho3 = else */
    | SE '(' Expr ')' ENTAO Comando SENAO Comando FIMSE
        { $$ = novoNo3(AST_CMD_SE_SENAO, yylineno, $3, $6, $8); }

    /* Laço: "enquanto(expr) cmd"
       filho1 = condição, filho2 = corpo do laço */
    | ENQUANTO '(' Expr ')' Comando
        { $$ = novoNo2(AST_CMD_ENQUANTO, yylineno, $3, $5); }

    /* Um bloco aninhado também é um comando */
    | Bloco
        { $$ = $1; }
    ;

/*
 * Expr — expressão ou atribuição.
 * A atribuição associa à direita: "x = y = 5" é válido.
 *   filho1 = identificador do lado esquerdo
 *   filho2 = expressão do lado direito
 */
Expr
    : OrExpr
        { $$ = $1; }
    | IDENTIFICADOR '=' Expr
        {
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, $1);
            $$ = novoNo2(AST_ATRIB, yylineno, id, $3);
            free($1);
        }
    ;

/*
 * OrExpr — operador lógico OU (||).
 * Associa à esquerda (recursão à esquerda na gramática).
 */
OrExpr
    : OrExpr OU AndExpr
        { $$ = novoNo2(AST_OU, yylineno, $1, $3); }
    | AndExpr
        { $$ = $1; }
    ;

/*
 * AndExpr — operador lógico E (&).
 */
AndExpr
    : AndExpr E EqExpr
        { $$ = novoNo2(AST_E, yylineno, $1, $3); }
    | EqExpr
        { $$ = $1; }
    ;

/*
 * EqExpr — operadores de igualdade (==) e diferença (!=).
 */
EqExpr
    : EqExpr IGUAL DesigExpr
        { $$ = novoNo2(AST_IGUAL, yylineno, $1, $3); }
    | EqExpr DIFERENTE DesigExpr
        { $$ = novoNo2(AST_DIFERENTE, yylineno, $1, $3); }
    | DesigExpr
        { $$ = $1; }
    ;

/*
 * DesigExpr — operadores relacionais (<, >, <=, >=).
 */
DesigExpr
    : DesigExpr '<' AddExpr
        { $$ = novoNo2(AST_MENOR, yylineno, $1, $3); }
    | DesigExpr '>' AddExpr
        { $$ = novoNo2(AST_MAIOR, yylineno, $1, $3); }
    | DesigExpr MAIORIGUAL AddExpr
        { $$ = novoNo2(AST_MAIORIGUAL, yylineno, $1, $3); }
    | DesigExpr MENORIGUAL AddExpr
        { $$ = novoNo2(AST_MENORIGUAL, yylineno, $1, $3); }
    | AddExpr
        { $$ = $1; }
    ;

/*
 * AddExpr — operadores aditivos (+ e -).
 */
AddExpr
    : AddExpr '+' MulExpr
        { $$ = novoNo2(AST_SOMA, yylineno, $1, $3); }
    | AddExpr '-' MulExpr
        { $$ = novoNo2(AST_SUB, yylineno, $1, $3); }
    | MulExpr
        { $$ = $1; }
    ;

/*
 * MulExpr — operadores multiplicativos (* e /).
 */
MulExpr
    : MulExpr '*' UnExpr
        { $$ = novoNo2(AST_MUL, yylineno, $1, $3); }
    | MulExpr '/' UnExpr
        { $$ = novoNo2(AST_DIV, yylineno, $1, $3); }
    | UnExpr
        { $$ = $1; }
    ;

/*
 * UnExpr — operadores unários (- e !).
 * filho1 = expressão sobre a qual o operador é aplicado.
 */
UnExpr
    : '-' PrimExpr
        { $$ = novoNo1(AST_NEG, yylineno, $2); }
    | '!' PrimExpr
        { $$ = novoNo1(AST_NAO, yylineno, $2); }
    | PrimExpr
        { $$ = $1; }
    ;

/*
 * PrimExpr — expressões primárias (folhas e agrupamento com parênteses).
 */
PrimExpr
    : IDENTIFICADOR
        { $$ = novoNo(AST_IDENTIFICADOR, yylineno, $1); free($1); }
    | CARCONST
        { $$ = novoNo(AST_CARCONST, yylineno, $1); free($1); }
    | INTCONST
        { $$ = novoNo(AST_INTCONST, yylineno, $1); free($1); }
    | '(' Expr ')'
        { $$ = $2; } /* parênteses apenas agrupam, não geram nó próprio */
    ;

/* ===== SEÇÃO DE CÓDIGO C ===== */
%%

/*
 * yyerror — chamada pelo Bison ao detectar um erro sintático.
 *
 * Imprime a mensagem de erro no formato exigido pelo trabalho:
 * "ERRO: <mensagem> proximo a '<token>' - linha: <número>"
 */
void yyerror(const char *s) {
    printf("ERRO: %s proximo a '%s' - linha: %d\n", s, yytext, yylineno);
}

extern No *raizAST;
TipoVar analisarSemantica(No *no, PilhaTabela *pilha);

/*
 * main — ponto de entrada do compilador.
 *
 * Abre o arquivo passado como argumento, executa a análise sintática
 * (que também constrói a AST), imprime a AST para depuração e libera
 * a memória ao final.
 *
 * Uso: ./g-v1 <arquivo.gv1>
 */
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo.gv1>\n", argv[0]);
        return 1;
    }

    /* abre o arquivo de entrada — yyin é usado internamente pelo Flex */
    if (!(yyin = fopen(argv[1], "r"))) {
        perror(argv[1]);
        return 1;
    }

    /* yyparse() dispara a análise sintática e, a cada regra reconhecida,
       as ações semânticas constroem os nós da AST */
    if (yyparse() == 0) {
        printf("Analise sintatica concluida com sucesso.\n");
        printf("\n--- AST ---\n");
        PilhaTabela *pilha = iniciarPilha();
        analisarSemantica(raizAST, pilha); /* análise semântica e checagem de tipos */
        imprimirAST(raizAST, 0); /* imprime a árvore para depuração */
        liberarAST(raizAST);    /* libera toda a memória da AST     */
        printf("\nArvore impressa e liberada\n\nAnalise semantica concluida com sucesso.\n");
    }

    fclose(yyin);
    return 0;
}
