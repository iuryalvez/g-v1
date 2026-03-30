// seção de declarações
%{ 
#include <stdio.h>
#include <stdlib.h>

/* declarações do lex */
extern int yylineno; // linha do arquivo de entrada
extern char* yytext; // token que acabou de ser lido
extern int yylex();
extern FILE* yyin;

/* função de erro usada pelo bison */
void yyerror(const char *s);
%}

/* declaração dos tokens */
%token PRINCIPAL INT CAR LEIA ESCREVA NOVALINHA ENQUANTO SE ENTAO SENAO FIMSE
%token IDENTIFICADOR CADEIACARACTERES CARCONST INTCONST
%token MENORIGUAL MAIORIGUAL IGUAL DIFERENTE E OU

// símbolo inicial
%start Programa

%%
// seção de regras da gramática
// LALR: look ahead left-to-right

Programa : DeclPrograma
         ;

DeclPrograma : PRINCIPAL Bloco
             ;

Bloco : '{' ListaComando '}'
      |  VarSection '{' ListaComando '}'
      ;

VarSection : '{' ListaDeclVar '}'
           ;

ListaDeclVar : IDENTIFICADOR DeclVar ':' Tipo ';' ListaDeclVar
             | IDENTIFICADOR DeclVar ':' Tipo ';'
             ;

DeclVar : // vazio
        | ',' IDENTIFICADOR DeclVar
        ;

Tipo : INT
     | CAR
     ;

ListaComando : Comando
             | Comando ListaComando
             ;

Comando : ';'
        | Expr ';'
        | LEIA IDENTIFICADOR ';'
        | ESCREVA Expr ';'
        | ESCREVA CADEIACARACTERES ';'
        | NOVALINHA ';'
        | SE '(' Expr ')' ENTAO Comando FIMSE
        | SE '(' Expr ')' ENTAO Comando SENAO Comando FIMSE
        | ENQUANTO '(' Expr ')' Comando
        | Bloco
        ;

Expr : OrExpr
     | IDENTIFICADOR '=' Expr
     ;

OrExpr : OrExpr OU AndExpr
       | AndExpr
       ;

AndExpr : AndExpr E EqExpr
        | EqExpr
        ;

EqExpr : EqExpr IGUAL DesigExpr
       | EqExpr DIFERENTE DesigExpr
       | DesigExpr
       ;

DesigExpr : DesigExpr '<' AddExpr
          | DesigExpr '>' AddExpr
          | DesigExpr MAIORIGUAL AddExpr
          | DesigExpr MENORIGUAL AddExpr
          | AddExpr
          ;

AddExpr : AddExpr '+' MulExpr
        | AddExpr '-' MulExpr
        | MulExpr
        ;

MulExpr : MulExpr '*' UnExpr
        | MulExpr '/' UnExpr
        | UnExpr
        ;

UnExpr : '-' PrimExpr
       | '!' PrimExpr
       | PrimExpr
       ;

PrimExpr : IDENTIFICADOR
         | CARCONST
         | INTCONST
         | '(' Expr ')'
         ;

%%

void yyerror(const char *s) {
    printf("ERRO: %s proximo a %s - linha: %d\n", s, yytext, yylineno);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
    }
    
    // parsing pelo bison
    if (yyparse() == 0) {
        printf("Analise sintatica concluida com sucesso.\n");
    }
    
    if (yyin) {
        fclose(yyin);
    }
    return 0;
}