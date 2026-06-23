/*
 * g-v2.y — Especificação do Analisador Sintático (Bison) para G-V2
 *
 * Este arquivo descreve a gramática da linguagem G-V2 no formato aceito
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
#include "ast.h"
#include "tabela.h"
#include "gerador.h"

extern int yylineno;
extern char *yytext;
extern int yylex();
extern FILE *yyin;

No *raizAST = NULL;

void yyerror(const char *s);
%}

%union {
    No   *no;
    char *str;
}

/* Tokens com valor léxico */
%token <str> IDENTIFICADOR CADEIACARACTERES CARCONST INTCONST

/* Palavras-reservadas e operadores */
%token PRINCIPAL INT CAR LEIA ESCREVA NOVALINHA ENQUANTO SE ENTAO SENAO FIMSE
%token MENORIGUAL MAIORIGUAL IGUAL DIFERENTE E OU
%token GLOBAL FUNCAO RETORNE

/* Tipagem dos nós da AST para os símbolos não-terminais */
%type <no> Programa DeclPrograma Bloco VarSection
%type <no> ListaDeclVar Tipo ListVar
%type <no> ListaComando Comando
%type <no> Expr OrExpr AndExpr EqExpr DesigExpr AddExpr MulExpr UnExpr PrimExpr
%type <no> DeclVarGlobais DeclFunc ListaFuncoes ListaParametros ListaParametrosTail
%type <no> LValueExpr ListExpr

%start Programa

/* ===== SEÇÃO DE REGRAS ===== */
%%

/* Programa: "global [...] funcao [...] principal [...]"
   filho1 = variáveis globais, filho2 = funções, filho3 = bloco principal */
Programa
    : DeclVarGlobais DeclFunc DeclPrograma
        { 
            /* Binds global vars, functions, and the main program into the root AST node */
            raizAST = novoNo3(AST_PROGRAMA, yylineno, $1, $2, $3); 
            $$ = raizAST; 
        }
    ;

/* DeclVarGlobais: "global [ var1: int; ]"
   filho1 = seção de variáveis declaradas (VarSection) ou nulo */
DeclVarGlobais
    : GLOBAL VarSection { $$ = novoNo1(AST_GLOBAIS, yylineno, $2); }
    | /* vazio */       { $$ = NULL; }
    ;

/* DeclPrograma: "principal { cmd; }"
   Retorna diretamente o nó do Bloco. */
DeclPrograma
    : PRINCIPAL Bloco
        { $$ = $2; }
    ;

/* Bloco delimitado por chaves
   Sem variáveis locais: filho1 = nulo, filho2 = comandos
   Com variáveis locais: filho1 = variáveis locais, filho2 = comandos */
Bloco
    : '{' ListaComando '}'
        { $$ = novoNo2(AST_BLOCO, yylineno, NULL, $2); }
    | VarSection '{' ListaComando '}'
        { $$ = novoNo2(AST_BLOCO, yylineno, $1, $3); }
    ;

/* VarSection: "[ var1: int; ]"
   Retorna diretamente a lista de declarações. */
VarSection
    : '[' ListaDeclVar ']'  { $$ = $2; }
    ;

/* ListaDeclVar: Encadeamento de linhas de declaração
   filho1 = lista de variáveis (ListVar) contendo o tipo, filho2 = próximas linhas */
ListaDeclVar
    : ListVar ':' Tipo ';' ListaDeclVar
        {
            /* The head of the ListVar chain receives the Type node in its filho2 */
            $1->filho2 = $3;
            $$ = novoNo2(AST_LISTA_DECL_VAR, yylineno, $1, $5);
        }
    | ListVar ':' Tipo ';'
        {
            $1->filho2 = $3;
            $$ = novoNo1(AST_LISTA_DECL_VAR, yylineno, $1);
        }
    ;

/* ListVar: Encadeamento de variáveis na mesma linha separadas por vírgula.
   Variável Simples: filho1 = identificador, filho3 = próxima variável da lista.
   Vetor: filho1 = nó AST_VETOR_DECL, filho3 = próxima variável da lista. */
ListVar
    : IDENTIFICADOR ',' ListVar
        {
            /* Creates a DECL_VAR wrapper for the simple identifier and links the tail */
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, $1);
            No *decl = novoNo2(AST_DECL_VAR, yylineno, id, NULL);
            decl->filho3 = $3;
            $$ = decl;
            free($1);
        }
    | IDENTIFICADOR '[' INTCONST ']' ',' ListVar
        {
            /* Instantiates an array declaration node and links it to the remaining list */
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, $1);
            No *dim = novoNo(AST_INTCONST, yylineno, $3);
            No *vet = novoNo2(AST_VETOR_DECL, yylineno, id, dim);

            No *decl = novoNo2(AST_DECL_VAR, yylineno, vet, NULL);
            decl->filho3 = $6;
            $$ = decl;
            free($1); free($3);
        }
    | IDENTIFICADOR
        {
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, $1);
            $$ = novoNo2(AST_DECL_VAR, yylineno, id, NULL);
            free($1);
        }
    | IDENTIFICADOR '[' INTCONST ']'
        {
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, $1);
            No *dim = novoNo(AST_INTCONST, yylineno, $3);
            No *vet = novoNo2(AST_VETOR_DECL, yylineno, id, dim);

            $$ = novoNo2(AST_DECL_VAR, yylineno, vet, NULL);
            free($1); free($3);
        }
    ;

/* DeclFunc: Escopo inicial das funções
   filho1 = primeira função encapsulada, filho2 = restante das funções. */
DeclFunc
    : FUNCAO '[' IDENTIFICADOR '(' ListaParametros ')' ':' Tipo Bloco ListaFuncoes ']'
        {
            /* Instantiates the first mandatory function of the block */
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, $3);
            No *func = novoNo4(AST_FUNCAO, yylineno, id, $8, $5, $9);
            $$ = novoNo2(AST_LISTA_FUNC, yylineno, func, $10);
            free($3);
        }
    | /* vazio */
        { $$ = NULL; }
    ;

/* ListaFuncoes: Encadeamento de funções adicionais
   filho1 = identificador, filho2 = tipo de retorno, filho3 = parâmetros, filho4 = bloco interno. */
ListaFuncoes
    : IDENTIFICADOR '(' ListaParametros ')' ':' Tipo Bloco ListaFuncoes
        {
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, $1);
            /* Function node layout: filho1=name, filho2=return_type, filho3=params, filho4=body */
            No *func = novoNo4(AST_FUNCAO, yylineno, id, $6, $3, $7);
            $$ = novoNo2(AST_LISTA_FUNC, yylineno, func, $8);
            free($1);
        }
    | /* vazio */ 
        { $$ = NULL; }
    ;

/* ListaParametros: Delega para a cauda recursiva ou retorna nulo. */
ListaParametros
    : ListaParametrosTail { $$ = $1; }
    | /* vazio */         { $$ = NULL; }
    ;

/* ListaParametrosTail: Encadeamento de parâmetros formais
   Parâmetro simples: filho1 = identificador, filho2 = tipo.
   Parâmetro vetor: filho1 = nó AST_VETOR_DECL, filho2 = tipo. */
ListaParametrosTail
    : IDENTIFICADOR ':' Tipo
        {
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, $1);
            No *param = novoNo2(AST_PARAMETRO, yylineno, id, $3);
            $$ = novoNo1(AST_LISTA_PARAM, yylineno, param);
            free($1);
        }
    | IDENTIFICADOR '[' ']' ':' Tipo
        {
            /* Array parameters in function signatures do not specify a dimension */
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, $1);
            No *vet = novoNo1(AST_VETOR_DECL, yylineno, id);
            No *param = novoNo2(AST_PARAMETRO, yylineno, vet, $5);
            $$ = novoNo1(AST_LISTA_PARAM, yylineno, param);
            free($1);
        }
    | IDENTIFICADOR ':' Tipo ',' ListaParametrosTail
        {
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, $1);
            No *param = novoNo2(AST_PARAMETRO, yylineno, id, $3);
            $$ = novoNo2(AST_LISTA_PARAM, yylineno, param, $5);
            free($1);
        }
    | IDENTIFICADOR '[' ']' ':' Tipo ',' ListaParametrosTail
        {
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, $1);
            No *vet = novoNo1(AST_VETOR_DECL, yylineno, id);
            No *param = novoNo2(AST_PARAMETRO, yylineno, vet, $5);
            $$ = novoNo2(AST_LISTA_PARAM, yylineno, param, $7);
            free($1);
        }
    ;

/* Tipo: Identificador de tipo primitivo
   Nó folha indicando INT ou CAR. */
Tipo
    : INT  { $$ = novoNo(AST_TIPO_INT, yylineno, NULL); }
    | CAR  { $$ = novoNo(AST_TIPO_CAR, yylineno, NULL); }
    ;

/* ListaComando: Encadeamento de instruções
   filho1 = comando atual, filho2 = próximo comando. */
ListaComando
    : Comando
        { $$ = novoNo1(AST_LISTA_COMANDO, yylineno, $1); }
    | Comando ListaComando
        { $$ = novoNo2(AST_LISTA_COMANDO, yylineno, $1, $2); }
    ;

/* Comando: Estruturas de controle e instruções atômicas */
Comando
    /* Comando Vazio: Ponto e vírgula solto. */
    : ';'
        { $$ = novoNo(AST_CMD_VAZIO, yylineno, NULL); }

    /* Expressão isolada: "expr;"
       filho1 = expressão avaliada. */
    | Expr ';'
        { $$ = novoNo1(AST_CMD_EXPR, yylineno, $1); }

    /* Retorno de função: "retorne expr;"
       filho1 = expressão de retorno. */
    | RETORNE Expr ';'
        { $$ = novoNo1(AST_CMD_RETORNE, yylineno, $2); }

    /* Leitura de variável: "leia x;" ou "leia v[0];"
       filho1 = nó LValueExpr destino. */
    | LEIA LValueExpr ';'
        { $$ = novoNo1(AST_CMD_LEIA, yylineno, $2); }

    /* Escrita de expressão: "escreva expr;"
       filho1 = expressão a ser exibida. */
    | ESCREVA Expr ';'
        { $$ = novoNo1(AST_CMD_ESCREVA_EXPR, yylineno, $2); }

    /* Escrita de cadeia literal: "escreva "texto";"
       Nó folha contendo o valor literal em string. */
    | ESCREVA CADEIACARACTERES ';'
        {
            $$ = novoNo(AST_CMD_ESCREVA_STR, yylineno, $2);
            free($2);
        }

    /* Quebra de linha de terminal: "novalinha;" */
    | NOVALINHA ';'
        { $$ = novoNo(AST_CMD_NOVALINHA, yylineno, NULL); }

    /* Condicional simples: "se (expr) entao cmd fimse"
       filho1 = condição avaliada, filho2 = comando (then). */
    | SE '(' Expr ')' ENTAO Comando FIMSE
        { $$ = novoNo2(AST_CMD_SE, yylineno, $3, $6); }

    /* Condicional composta: "se (expr) entao cmd senao cmd fimse"
       filho1 = condição, filho2 = comando (then), filho3 = comando (else). */
    | SE '(' Expr ')' ENTAO Comando SENAO Comando FIMSE
        { $$ = novoNo3(AST_CMD_SE_SENAO, yylineno, $3, $6, $8); }

    /* Laço: "enquanto (expr) cmd"
       filho1 = condição de controle, filho2 = corpo da repetição. */
    | ENQUANTO '(' Expr ')' Comando
        { $$ = novoNo2(AST_CMD_ENQUANTO, yylineno, $3, $5); }

    /* Bloco aninhado tratado como comando. */
    | Bloco
        { $$ = $1; }
    ;

/* Expr: Avaliações gerais ou atribuição
   Atribuição: filho1 = destino (LValue), filho2 = valor calculado. */
Expr
    : LValueExpr '=' Expr
        { $$ = novoNo2(AST_ATRIB, yylineno, $1, $3); }
    | OrExpr
        { $$ = $1; }
    ;

/* LValueExpr: Variável destino de atribuição/leitura
   Acesso a vetor: filho1 = identificador, filho2 = expressão do índice.
   Variável simples: filho1 = identificador. */
LValueExpr
    : IDENTIFICADOR '[' Expr ']'
        {
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, $1);
            $$ = novoNo2(AST_VETOR_ACESSO, yylineno, id, $3);
            free($1);
        }
    | IDENTIFICADOR
        {
            $$ = novoNo(AST_IDENTIFICADOR, yylineno, $1);
            free($1);
        }
    ;

/* OrExpr: Lógico OU (||)
   filho1 = operando esquerdo, filho2 = operando direito. */
OrExpr
    : OrExpr OU AndExpr
        { $$ = novoNo2(AST_OU, yylineno, $1, $3); }
    | AndExpr
        { $$ = $1; }
    ;

/* AndExpr: Lógico E (&)
   filho1 = operando esquerdo, filho2 = operando direito. */
AndExpr
    : AndExpr E EqExpr
        { $$ = novoNo2(AST_E, yylineno, $1, $3); }
    | EqExpr
        { $$ = $1; }
    ;

/* EqExpr: Igualdade (==) e Diferença (!=)
   filho1 = operando esquerdo, filho2 = operando direito. */
EqExpr
    : EqExpr IGUAL DesigExpr
        { $$ = novoNo2(AST_IGUAL, yylineno, $1, $3); }
    | EqExpr DIFERENTE DesigExpr
        { $$ = novoNo2(AST_DIFERENTE, yylineno, $1, $3); }
    | DesigExpr
        { $$ = $1; }
    ;

/* DesigExpr: Relacionais (<, >, <=, >=)
   filho1 = operando esquerdo, filho2 = operando direito. */
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

/* AddExpr: Adição e Subtração (+, -)
   filho1 = operando esquerdo, filho2 = operando direito. */
AddExpr
    : AddExpr '+' MulExpr
        { $$ = novoNo2(AST_SOMA, yylineno, $1, $3); }
    | AddExpr '-' MulExpr
        { $$ = novoNo2(AST_SUB, yylineno, $1, $3); }
    | MulExpr
        { $$ = $1; }
    ;

/* MulExpr: Multiplicação e Divisão (*, /)
   filho1 = operando esquerdo, filho2 = operando direito. */
MulExpr
    : MulExpr '*' UnExpr
        { $$ = novoNo2(AST_MUL, yylineno, $1, $3); }
    | MulExpr '/' UnExpr
        { $$ = novoNo2(AST_DIV, yylineno, $1, $3); }
    | UnExpr
        { $$ = $1; }
    ;

/* UnExpr: Unários (Negação aritmética e lógica)
   filho1 = operando a ser negado ou invertido. */
UnExpr
    : '-' PrimExpr
        { $$ = novoNo1(AST_NEG, yylineno, $2); }
    | '!' PrimExpr
        { $$ = novoNo1(AST_NAO, yylineno, $2); }
    | PrimExpr
        { $$ = $1; }
    ;

/* PrimExpr: Expressões atômicas, agrupamentos e chamadas
   Chamada de Função: filho1 = identificador, filho2 = lista de argumentos (ListExpr). */
PrimExpr
    : IDENTIFICADOR '(' ListExpr ')'
        {
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, $1);
            $$ = novoNo2(AST_CHAMADA_FUNC, yylineno, id, $3);
            free($1);
        }
    | IDENTIFICADOR '(' ')'
        {
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, $1);
            $$ = novoNo1(AST_CHAMADA_FUNC, yylineno, id);
            free($1);
        }
    /* Delega para resolução de variável ou vetor. */
    | LValueExpr
        { $$ = $1; }
    /* Constante literal de caractere. */
    | CARCONST
        { $$ = novoNo(AST_CARCONST, yylineno, $1); free($1); }
    /* Constante literal inteira. */
    | INTCONST
        { $$ = novoNo(AST_INTCONST, yylineno, $1); free($1); }
    /* Expressão agrupada em parênteses (retorna diretamente o filho). */
    | '(' Expr ')'
        { $$ = $2; }
    ;

/* ListExpr: Encadeamento de argumentos passados na chamada
   filho1 = expressão do argumento atual, filho2 = próximo argumento. */
ListExpr
    : Expr
        { $$ = novoNo1(AST_LISTA_EXPR, yylineno, $1); }
    | ListExpr ',' Expr
        {
            /* Left-recursive execution: appends the evaluated expression to the argument list */
            $$ = novoNo2(AST_LISTA_EXPR, yylineno, $1, $3);
        }
    ;

/* ===== SEÇÃO DE CÓDIGO C ===== */
%%

void yyerror(const char *s) {
    printf("ERRO: %s proximo a '%s' - linha: %d\n", s, yytext, yylineno);
}

extern No *raizAST;
TipoVar analisarSemantica(No *no, PilhaTabela *pilha);

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo.gv2>\n", argv[0]);
        return 1;
    }

    if (!(yyin = fopen(argv[1], "r"))) {
        perror(argv[1]);
        return 1;
    }

    if (yyparse() == 0) {
        printf("Analise sintatica concluida com sucesso.\n");
        printf("\n--- AST ---\n");
        
        // PilhaTabela *pilha = iniciarPilha();
        // analisarSemantica(raizAST, pilha);
        // liberarPilha(pilha);
        // printf("Analise semantica concluida com sucesso.\n");

        // char nomeSaida[512];
        // strncpy(nomeSaida, argv[1], sizeof(nomeSaida) - 3);
        // char *ponto = strrchr(nomeSaida, '.');
        // if (ponto) *ponto = '\0';
        // strcat(nomeSaida, ".s");

        // FILE *arquivoSaida = fopen(nomeSaida, "w");
        // if (!arquivoSaida) {
        //     perror(nomeSaida);
        //     return 1;
        // }
        // gerarCodigo(raizAST, arquivoSaida);
        // fclose(arquivoSaida);
        // printf("Codigo MIPS gerado em: %s\n", nomeSaida);

        imprimirAST(raizAST, 0);
        liberarAST(raizAST);
    }

    fclose(yyin);
    return 0;
}