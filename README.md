# Compilador G-V1

## Descrição
Implementação de um compilador didático para a linguagem G-V1. O projeto utiliza Flex para análise léxica e Bison para análise sintática.

## Dependências
O ambiente de avaliação utilizará as seguintes versões:
- GCC/G++ versão 14
- Flex versão 2.6.4
- Bison versão 3.8.2

## Estrutura do Projeto
- `g-v1.l`: Especificação do analisador léxico.
- `testes/teste.gv1`: Arquivo de teste da linguagem.
- `Makefile`: Script de automação de compilação.

## Execução

Gere o executável do compilador através do comando:
```bash
make
```

## Instruções do Flex

### Slides 3-5 sobre o Flex

Flex e Lex são programas geradores de analisadores léxicos.

Recebem como entrada um arquivo contendo expressões
regulares correspondentes aos padrões a serem reconhecidos
pelo analisador léxico a ser gerado.

Geram um arquivo em C contendo o código do analisador léxico gerado.

O analisador é formado por um conjunto de autômatos finitos.

Um autômato é gerado para cada expressão regular. (Teorema de Thompson)

### Slides 8-9 sobre a seção de definições

Nesta seção são especificadas as definições regulares. Elas possuem
o seguinte formato: nome expressão-regular, onde nome deve
iniciar com letra ou "_" e pode conter zero ou mais: letra, digito,
"-" e "_". A expressão regular inicia após o primeiro caractere não
branco (espaço, tabulação) após nome. Ex.: digit [0-9].

### Slide 16 sobre patterns

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

### Slide 8 sobre options

As opções para o comando flex podem ser passadas ao programa
na linha de comando (ex. fex –header-file="lexico.h") ou
podem ser informadas nesta seção através da diretiva %option. Por
exemplo: %option header-file="lexico.h".

### Slide 10 sobre a definição de regras (após o primeiro %%)

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

### Slide 13 sobre start conditions:
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

### Slide 19 sobre o texto após o segundo %%

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
