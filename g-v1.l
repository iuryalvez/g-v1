%{

/* slides 3-5 sobre o Flex:
 Flex e Lex são programas geradores de analisadores léxicos.
 Recebem como entrada um arquivo contendo expressões
 regulares correspondentes aos padrões a serem reconhecidos
 pelo analisador léxico a ser gerado.
 Geram um arquivo em C contendo o código do analisador léxico gerado.
 O analisador é formado por um conjunto de autômatos finitos.
 Um autômato é gerado para cada expressão regular. (Teorema de Thompson)
*/ 

/* seção de definições de variáveis da linguagem C e arquivos de cabeçalho */

/* Um texto identado ou um texto delimitado por %{ e %} é copiado para o arquivo de saída (em C): */
// header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "g-v1.h"
%}


/* slides 8-9 sobre a seção de definições:
Nesta seção são especificadas as definições regulares. Elas possuem
o seguinte formato: nome expressão-regular, onde nome deve
iniciar com letra ou "_" e pode conter zero ou mais: letra, digito,
"-" e "_". A expressão regular inicia após o primeiro caractere não
branco (espaço, tabulação) após nome. Ex.: digit [0-9].
*/


/* slide 16 sobre patterns
No manual do Flex na seção Patterns há uma série de simplificações
de expressões regulares que podem ajudar na escrita das expressões
regulares.Entre elas destacam-se:

Classes de caracteres: [xyz] = x|y|z, [ˆxyz] =ˆ(x|y|z),
[abj − mZ] = (a|b|j|k|l|m|Z).

Expressões de classes de caracteres: [:alnum:], [:alpha:],
[:digit:],[:space:],[:lower:], [:upper:] que devem parecer em
classes de caracteres, i.e. entre [ e ]. Exemplo: as expressões a
seguir são equivalentes entre si: [[:alnum:]]= [[:alpha:][:digit:]]
= [[:alpha:][0-9]] = [a-zA-Z0-9]
*/

/* token: DIGITO; lexemas: D-I-G-I-T-O */
DIGITO              [0-9]
LETRA               [a-zA-Z]
IDENTIFICADOR       ({LETRA}|_)({LETRA}|{DIGITO}|_)*
CADEIACARACTERES    \"[^\"\n]*\"
ERRO_CADEIA         \"[^\"\n]*\n
CARCONST            \'([^'\\\n]|\\.)\'
INTCONST            {DIGITO}+

/* slide 8 sobre options
As opções para o comando flex podem ser passadas ao programa
na linha de comando (ex. fex –header-file="lexico.h") ou
podem ser informadas nesta seção através da diretiva %option. Por
exemplo: %option header-file="lexico.h".
*/

%option yylineno noyywrap
/* %option header-file="lexico.h" */

/* declaração de uma start condition */
%x COMENTARIO

/* seção de regras de expressão regular e ações associadas (após o primeiro %%):

slide 10 sobre regras:
Uma regra tem o seguinte formato:
padrão ação

Não pode haver identação antes de padrão

A ação deve aparecer na mesma linha de padrão, separados
por pelo menos um espaço ou tabulação.

Caso uma ação corresponda a várias linhas de código em C,
ela deve iniciar com { e terminar com }.

Se uma definição regular é usada no padrão de uma regra ela
deve aparecer entre chaves para indicar que ela deve ser
substituída pela expressão regular correspondente. Por
exemplo: {DIGIT}+ return NUMERO usa a definição regular
DIGIT definida na seção de declarações. 

slide 13 sobre start conditions:
Na seção de definição (antes do primeiro %%) definimos uma
condição de partida: %x comentario

Na seção de regras, definimos uma expressão regular que
caracteriza o início de um contexto. No caso, a expressão:
"/*". Acrescentamos a ação BEGIN(comentario); 

Regra completa: "/*" {BEGIN(comentario);}. Indica que, ao
encontrar /* na entrada, o paser entra no contexto
comentário.

As regras seguintes que devem valer no contexto c criado,
devem ser escritas na forma:
<c>expressão ação.

Uma expressão de término do contexto pode ser especificada
para marcar o fim de um contexto. 

A ação: BEGIN(INITIAL); é utilizada para indicar ao parser para
voltar ao contexto normal (denominado INITIAL).
*/

%%

"/*"                { BEGIN(COMENTARIO); } /* Inicia o estado de comentário */
<COMENTARIO>"*/"    { BEGIN(INITIAL); }    /* Retorna ao estado inicial */
<COMENTARIO>\n      { /* flex atualiza a yylineno automaticamente */ }
<COMENTARIO>.       { /* ignora qualquer outro caractere no comentário */ }
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
"|"                 { return OU; }

{IDENTIFICADOR}     { return IDENTIFICADOR; }
{CADEIACARACTERES}  { return CADEIACARACTERES; }
{CARCONST}          { return CARCONST; }
{INTCONST}          { return INTCONST; }

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
