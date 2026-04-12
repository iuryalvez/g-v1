# Compilador G-V1

Implementação de um compilador didático para a linguagem **G-V1**, desenvolvido
para a disciplina de Compiladores da Universidade Federal de Goiás.

---

## Dependências

| Ferramenta | Versão usada |
|------------|-------------|
| GCC/G++    | 14          |
| Flex       | 2.6.4       |
| Bison      | 3.8.2       |

Para instalar no Ubuntu/Debian:
```bash
sudo apt install flex bison gcc make
```

---

## Estrutura do Projeto

```
g-v1/
├── g-v1.l       # Analisador léxico (Flex)
├── g-v1.y       # Analisador sintático + construção da AST (Bison)
├── ast.h        # Definição dos tipos e funções da AST
├── ast.c        # Implementação da AST
├── Makefile     # Script de compilação
└── testes/
    └── teste.gv1
```

---

## Como compilar e executar

```bash
# Compila tudo (léxico + sintático + AST → executável g-v1)
make

# Executa o compilador em um arquivo de entrada
./g-v1 testes/teste.gv1

# Remove arquivos gerados
make clean
```

---

## Fases implementadas

### ✅ Fase 1 — Analisador Léxico (`g-v1.l`)

Gerado automaticamente pelo **Flex** a partir das expressões regulares definidas
no arquivo `g-v1.l`. Reconhece todos os tokens da linguagem G-V1:

- Palavras reservadas: `principal`, `int`, `car`, `leia`, `escreva`, `novalinha`,
  `se`, `entao`, `senao`, `fimse`, `enquanto`
- Identificadores, constantes inteiras, constantes de caractere e strings
- Operadores aritméticos, relacionais e lógicos
- Comentários no estilo `/* ... */` (podem ocupar várias linhas)
- Detecção e reporte de erros léxicos:
  - `ERRO: CARACTERE INVALIDO <linha>`
  - `ERRO: COMENTARIO NAO TERMINA <linha>`
  - `ERRO: CADEIA DE CARACTERES OCUPA MAIS DE UMA LINHA <linha>`

**Mudança introduzida (Fase 2):** Os tokens que carregam valor léxico
(`IDENTIFICADOR`, `INTCONST`, `CARCONST`, `CADEIACARACTERES`) passaram a
preencher `yylval.str` com uma cópia do texto (`strdup(yytext)`), para que o
Bison possa acessar o lexema nas ações semânticas.

---

### ✅ Fase 2 — Analisador Sintático (`g-v1.y`)

Gerado automaticamente pelo **Bison** (método LALR(1)) a partir da gramática
da linguagem G-V1 definida no arquivo `g-v1.y`.

Reconhece a estrutura completa do programa: blocos aninhados, declarações de
variáveis, comandos (`leia`, `escreva`, `se`, `enquanto`, etc.) e expressões
com a precedência correta de operadores.

Erros sintáticos são reportados com o formato:
```
ERRO: <mensagem> proximo a '<token>' - linha: <número>
```

---

### ✅ Fase 3 — Geração da AST (`ast.h` / `ast.c`)

A **Árvore Sintática Abstrata (AST)** é construída durante a análise sintática,
por meio de ações semânticas embutidas em cada regra da gramática no `g-v1.y`.

#### Por que uma AST?

A AST é uma representação compacta e estruturada do programa. Ela elimina
detalhes sintáticos desnecessários (como parênteses e ponto-e-vírgula) e mantém
apenas a estrutura lógica do código, que será usada pelas fases seguintes
(análise semântica e geração de código).

#### Estrutura de um nó

Cada nó da AST é representado pela struct `No`:

```c
typedef struct No {
    TipoNo tipo;      // que construção da linguagem este nó representa
    int linha;        // linha no fonte (para mensagens de erro)
    char *valor;      // texto léxico (apenas em nós folha)
    struct No *filho1;
    struct No *filho2;
    struct No *filho3;
} No;
```

#### Tipos de nós

| Categoria       | Tipos de nó                                                   |
|-----------------|---------------------------------------------------------------|
| Programa        | `AST_PROGRAMA`, `AST_BLOCO`                                   |
| Declarações     | `AST_LISTA_DECL_VAR`, `AST_DECL_VAR`, `AST_TIPO_INT/CAR`     |
| Comandos        | `AST_CMD_VAZIO/EXPR/LEIA/ESCREVA_*/NOVALINHA/SE/SE_SENAO/ENQUANTO` |
| Expressões      | `AST_ATRIB`, `AST_SOMA/SUB/MUL/DIV`, `AST_OU/E/IGUAL/...`   |
| Folhas          | `AST_IDENTIFICADOR`, `AST_INTCONST`, `AST_CARCONST`, `AST_CADEIACARACTERES` |

#### Exemplo de saída (programa simples)

Dado o programa:
```
principal
{
  { x : int; }
  {
    x = 5;
    escreva x;
    novalinha;
  }
}
```

A AST impressa é:
```
[PROGRAMA] (linha 9)
  [BLOCO] (linha 9)
    [LISTA_COMANDO] (linha 9)
      [BLOCO] (linha 8)
        [LISTA_DECL_VAR] (linha 3)
          [DECL_VAR] (linha 3)
            [IDENTIFICADOR] "x" (linha 3)
            [TIPO_INT] (linha 3)
        [LISTA_COMANDO] (linha 8)
          [CMD_EXPR] (linha 5)
            [ATRIB] (linha 5)
              [IDENTIFICADOR] "x" (linha 5)
              [INTCONST] "5" (linha 5)
          [LISTA_COMANDO] (linha 8)
            [CMD_ESCREVA_EXPR] (linha 6)
              [IDENTIFICADOR] "x" (linha 6)
            [LISTA_COMANDO] (linha 8)
              [CMD_NOVALINHA] (linha 7)
```

---

## Próximas fases (a implementar)

- [ ] **Tabela de Símbolos** — pilha de escopos para controle de variáveis
- [ ] **Análise Semântica** — verificação de tipos e escopos sobre a AST
- [ ] **Geração de Código** — geração de assembly MIPS a partir da AST
