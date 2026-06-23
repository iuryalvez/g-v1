/*
 * g-v2.l — Especificação do Analisador Léxico (Flex) para G-V2
 *
 * Este arquivo contém as expressões regulares para identificar os lexemas
 * da linguagem G-V2. Quando um padrão é casado, o analisador executa a
 * ação em C correspondente, geralmente retornando um Token para o Bison.
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
/* Arquivo gerado pelo Bison contendo a definição dos tokens (%token) */
#include "g-v2.h" 
%}

/* ===== DEFINIÇÕES REGULARES ===== */

/* Dígitos de 0 a 9 */
DIGITO              [0-9]

/* Letras maiúsculas e minúsculas */
LETRA               [a-zA-Z]

/* Identificador: Inicia com letra ou '_', seguido por letras, dígitos ou '_' */
IDENTIFICADOR       ({LETRA}|_)({LETRA}|{DIGITO}|_)*

CADEIACARACTERES    \"[^\"\n]*\"

ERRO_CADEIA         \"[^\"\n]*\n

CARCONST            \'([^'\\\n]|\\.)\'

/* Constante Inteira: Um ou mais dígitos */
INTCONST            {DIGITO}+

/* ===== OPÇÕES DO FLEX ===== */
%option yylineno noyywrap
%option nounput noinput

/* Condição de partida para tratar comentários de múltiplas linhas */
%x COMENTARIO

/* ===== SEÇÃO DE REGRAS E AÇÕES ===== */
%%

"/*"                { BEGIN(COMENTARIO); } 
<COMENTARIO>"*/"    { BEGIN(INITIAL); }    
<COMENTARIO>\n      { /* flex atualiza a yylineno automaticamente */ }
<COMENTARIO>.       { /* ignora qualquer outro caractere dentro do comentário */ }
<COMENTARIO><<EOF>> { 
    printf("ERRO: COMENTÁRIO NAO TERMINA %d\n", yylineno);
    exit(1);
}

"principal"         { return PRINCIPAL; }

"int"               { return INT; }
"car"               { return CAR; }

"leia"              { return LEIA; }

"escreva"           { return ESCREVA; }

"novalinha"         { return NOVALINHA; }

"enquanto"          { return ENQUANTO; }

"se"                { return SE; }
"entao"             { return ENTAO; }
"senao"             { return SENAO; }
"fimse"             { return FIMSE; }

"+"                 { return '+'; }
"-"                 { return '-'; }
"*"                 { return '*'; }
"/"                 { return '/'; }

"!"                 { return '!'; }

"{"                 { return '{'; }
"}"                 { return '}'; }

"("                 { return '('; }
")"                 { return ')'; }

";"                 { return ';'; }
":"                 { return ':'; }
","                 { return ','; }

"="                 { return '='; }

"<"                 { return '<'; }
">"                 { return '>'; }
"<="                { return MENORIGUAL; }
">="                { return MAIORIGUAL; }
"=="                { return IGUAL; }
"!="                { return DIFERENTE; }

"&"                 { return E; }
"||"                { return OU; }

"global"    { return GLOBAL; }

"funcao"    { return FUNCAO; }

"retorne"   { return RETORNE; }

"["         { return '['; }
"]"         { return ']'; }

{IDENTIFICADOR}      { yylval.str = strdup(yytext); return IDENTIFICADOR; }
{CADEIACARACTERES}   { yylval.str = strdup(yytext); return CADEIACARACTERES; }
{CARCONST}           { yylval.str = strdup(yytext); return CARCONST; }
{INTCONST}           { yylval.str = strdup(yytext); return INTCONST; }

{ERRO_CADEIA}       { 
    printf("ERRO: CADEIA DE CARACTERES OCUPA MAIS DE UMA LINHA %d\n", yylineno);
    exit(1); 
}

\n                  { /* flex atualiza a yylineno automaticamente */ }
[ \t\r]+            { /* consome espaços em branco */ }

.                   { 
    printf("ERRO: CARACTERE INVÁLIDO %d\n", yylineno);
    exit(1);
}

%%

/* ===== SEÇÃO DE CÓDIGO C ADICIONAL ===== */
/* (Opcional, usado para funções auxiliares nativas do Lexer se necessário) */

/* slide 19 sobre o texto após o segundo %%
O texto que aparece nessa seção é copiado literalmente para o
arquivo de saída na linguagem C gerado pelo Flex (i.e., lex.yy.c).
Esta seção é geralmente formada por funções criadas pelo usuário
para chamarem ou serem chamadas por yylex().

A presença desta seção é opcional. No slide a seguir, um arquivo
separado em C (main.c) é usado para abrir um arquivo de teste e
ficar chamando yylex() para encontrar tokens no mesmo. Porém,
poderíamos ter colocado a função main() que está em main.c, nesta
seção e teríamos o mesmo efeito... 

Notas: 
Com isso, usando essa seção, é possível testar o Lex sem o Bison com a função main() declarada aqui.
O código funciona normalmente, mas é preciso declarar os tokens com #define 
na seção de definições (entre os "%{ %}" ) para o código compilar:

// #define PRINCIPAL 258
// #define INT 259
// #define CAR 260
// #define LEIA 261
// #define ESCREVA 262
// #define NOVALINHA 263
// #define ENQUANTO 264
// #define SE 265
// #define ENTAO 266
// #define SENAO 267
// #define FIMSE 268
// #define MENORIGUAL 269
// #define MAIORIGUAL 270
// #define IGUAL 271
// #define DIFERENTE 272
// #define E 273
// #define OU 274
// #define IDENTIFICADOR 275
// #define CADEIACARACTERES 276
// #define CARCONST 277
// #define INTCONST 278

// código C para testar sem o bison:

extern FILE* yyin;

int main(int argc, char **argv) {
    if(argc > 1) {
        if(!(yyin = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
    }
    while(yylex() != 0);

    if (yyin) {
        fclose(yyin);
    }
    return 0;
}
*/
