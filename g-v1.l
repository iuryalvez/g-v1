%{
// seção do Flex de definição de variáveis da linguagem C e arquivos de cabeçalho
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "g-v1.h"
// DECLARAÇÕES TEMPORÁRIAS PARA TESTE DO FLEX
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
%}

/* seção do Flex destinada a definições regulares (MACROS) */
DIGITO              [0-9]
LETRA               [a-zA-Z]
IDENTIFICADOR       ({LETRA}|_)({LETRA}|{DIGITO}|_)*
CADEIACARACTERES    \"[^\"\n]*\"
ERRO_CADEIA         \"[^\"\n]*\n
CARCONST            \'([^'\\\n]|\\.)\'
INTCONST            {DIGITO}+

%option noyywrap
%option yylineno

/* define o estado exclusivo para lidar com comentários de múltiplas linhas */
%x COMENTARIO

%%

"/*"                { BEGIN(COMENTARIO); } // Inicia o estado de comentário
<COMENTARIO>"*/"    { BEGIN(INITIAL); }    // Retorna ao estado inicial
<COMENTARIO>\n      { /* flex atualiza a yylineno automaticamente */ }
<COMENTARIO>.       { /* ignora qualquer outro caractere no comentário */ }
<COMENTARIO><<EOF>> { 
    printf("ERRO: COMENTÁRIO NAO TERMINA %d\n", yylineno);
    exit(1);
}

"principal"         { printf("[TOKEN]: 'principal' | [LINHA]: %d\n", yylineno); return PRINCIPAL; }

"int"               { printf("[TOKEN]: 'int' | [LINHA]: %d\n", yylineno); return INT; }
"car"               { printf("[TOKEN]: 'car' | [LINHA]: %d\n", yylineno); return CAR; }

"leia"              { printf("[TOKEN]: 'leia' | [LINHA]: %d\n", yylineno); return LEIA; }
"escreva"           { printf("[TOKEN]: 'escreva' | [LINHA]: %d\n", yylineno); return ESCREVA; }
"novalinha"         { printf("[TOKEN]: 'novalinha' | [LINHA]: %d\n", yylineno); return NOVALINHA; }

"enquanto"          { printf("[TOKEN]: 'enquanto' | [LINHA]: %d\n", yylineno); return ENQUANTO; }

"se"                { printf("[TOKEN]: 'se' | [LINHA]: %d\n", yylineno); return SE; }
"entao"             { printf("[TOKEN]: 'entao' | [LINHA]: %d\n", yylineno); return ENTAO; }
"senao"             { printf("[TOKEN]: 'senao' | [LINHA]: %d\n", yylineno); return SENAO; }
"fimse"             { printf("[TOKEN]: 'fimse' | [LINHA]: %d\n", yylineno); return FIMSE; }

"+"                 { printf("[TOKEN]: '+' | [LINHA]: %d\n", yylineno); return '+'; }
"-"                 { printf("[TOKEN]: '-' | [LINHA]: %d\n", yylineno); return '-'; }
"*"                 { printf("[TOKEN]: '*' | [LINHA]: %d\n", yylineno); return '*'; }
"/"                 { printf("[TOKEN]: '/' | [LINHA]: %d\n", yylineno); return '/'; }
"!"                 { printf("[TOKEN]: '!' | [LINHA]: %d\n", yylineno); return '!'; }

"{"                 { printf("[TOKEN]: '{' | [LINHA]: %d\n", yylineno); return '{'; }
"}"                 { printf("[TOKEN]: '}' | [LINHA]: %d\n", yylineno); return '}'; }
"("                 { printf("[TOKEN]: '(' | [LINHA]: %d\n", yylineno); return '('; }
")"                 { printf("[TOKEN]: ')' | [LINHA]: %d\n", yylineno); return ')'; }

";"                 { printf("[TOKEN]: ';' | [LINHA]: %d\n", yylineno); return ';'; }
":"                 { printf("[TOKEN]: ':' | [LINHA]: %d\n", yylineno); return ':'; }
","                 { printf("[TOKEN]: ',' | [LINHA]: %d\n", yylineno); return ','; }

"="                 { printf("[TOKEN]: '=' | [LINHA]: %d\n", yylineno); return '='; }
"<"                 { printf("[TOKEN]: '<' | [LINHA]: %d\n", yylineno); return '<'; }
">"                 { printf("[TOKEN]: '>' | [LINHA]: %d\n", yylineno); return '>'; }
"<="                { printf("[TOKEN]: '<=' | [LINHA]: %d\n", yylineno); return MENORIGUAL; }
">="                { printf("[TOKEN]: '>=' | [LINHA]: %d\n", yylineno); return MAIORIGUAL; }
"=="                { printf("[TOKEN]: '==' | [LINHA]: %d\n", yylineno); return IGUAL; }
"!="                { printf("[TOKEN]: '!=' | [LINHA]: %d\n", yylineno); return DIFERENTE; }

"&"                 { printf("[TOKEN]: '&' | [LINHA]: %d\n", yylineno); return E; }
"|"                 { printf("[TOKEN]: '|' | [LINHA]: %d\n", yylineno); return OU; }

{IDENTIFICADOR}     { printf("[TOKEN]: 'identificador' (%s) | [LINHA]: %d\n", yytext, yylineno); return IDENTIFICADOR; }
{CADEIACARACTERES}  { printf("[TOKEN]: 'cadeiaCaracteres' (%s) | [LINHA]: %d\n", yytext, yylineno); return CADEIACARACTERES; }
{CARCONST}          { printf("[TOKEN]: 'carConst' (%s) | [LINHA]: %d\n", yytext, yylineno); return CARCONST; }
{INTCONST}          { printf("[TOKEN]: 'intConst' (%s) | [LINHA]: %d\n", yytext, yylineno); return INTCONST; }

{ERRO_CADEIA}       { 
    printf("ERRO: CADEIA DE CARACTERES OCUPA MAIS DE UMA LINHA %d\n", yylineno);
    exit(1); 
}

\n                  { /* flex atualiza a yylineno automaticamente */ }

[ \t\r]+            { /* consome espaços em branco */ }

.                   { 
    /* fallback rule para qualquer caractere inválido */
    printf("ERRO: CARACTERE INVÁLIDO %d\n", yylineno);
    exit(1);
}

%%

// RETIRAR APÓS O BISON ESTAR IMPLEMENTADO
// Declaração externa de yyin
//extern FILE* yyin;

// int main(int argc, char **argv) {
//     if(argc > 1) {
//         if(!(yyin = fopen(argv[1], "r"))) {
//             perror(argv[1]);
//             return 1;
//         }
//     }
//     while(yylex() != 0);

//     if (yyin) {
//         fclose(yyin);
//     }
//     return 0;
// }