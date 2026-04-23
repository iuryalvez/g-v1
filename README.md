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
├── g-v1.l         # Analisador léxico (Flex)
├── g-v1.y         # Analisador sintático + construção da AST (Bison)
├── ast.h          # Definição dos tipos e funções da AST
├── ast.c          # Implementação da AST
├── tabela.h       # Definição da tabela de símbolos (pilha de escopos)
├── tabela.c       # Implementação da tabela de símbolos
├── semantico.c    # Analisador semântico (percurso na AST)
├── gerador.h      # Definição do gerador de código MIPS
├── gerador.c      # Implementação do gerador de código MIPS
├── Makefile       # Script de compilação
└── Testes-G.v1/
    ├── Corretos/         # Programas sem erros
    ├── ErroSintatico/    # Programas com erros sintáticos intencionais
    └── ErroSemantico/    # Programas com erros semânticos intencionais
```

---

## Como compilar e executar

```bash
# Compila tudo e gera o executável g-v1
make

# Executa o compilador em um arquivo de entrada
./g-v1 programa.g

# Remove arquivos gerados (objetos, executável, assembly)
make clean

# Executa a bateria de testes
make test
```

Se o programa estiver correto, o compilador:
1. Imprime `Analise sintatica concluida com sucesso.`
2. Imprime `Analise semantica concluida com sucesso.`
3. Gera um arquivo `.s` com o assembly MIPS (mesmo nome do `.g`)

---

## Fases implementadas

### ✅ Fase 1 — Analisador Léxico (`g-v1.l`)

Gerado automaticamente pelo **Flex**. Lê o arquivo de entrada caractere por
caractere e agrupa os símbolos em tokens — a menor unidade com significado
na linguagem (palavras reservadas, identificadores, operadores, constantes).

Tokens reconhecidos:
- Palavras reservadas: `principal`, `int`, `car`, `leia`, `escreva`,
  `novalinha`, `se`, `entao`, `senao`, `fimse`, `enquanto`
- Identificadores, constantes inteiras, constantes de caractere e strings
- Operadores aritméticos, relacionais e lógicos
- Comentários `/* ... */` (descartados, podem ocupar várias linhas)

Erros léxicos reportados:
```
ERRO: CARACTERE INVALIDO <linha>
ERRO: COMENTARIO NAO TERMINA <linha>
ERRO: CADEIA DE CARACTERES OCUPA MAIS DE UMA LINHA <linha>
```

---

### ✅ Fase 2 — Analisador Sintático (`g-v1.y`)

Gerado automaticamente pelo **Bison** (método LALR(1)). Verifica se a sequência
de tokens respeita as regras gramaticais da linguagem G-V1.

Erros sintáticos são reportados com o formato:
```
ERRO: <mensagem> proximo a '<token>' - linha: <número>
```

---

### ✅ Fase 3 — Geração da AST (`ast.h` / `ast.c`)

A **Árvore Sintática Abstrata (AST)** é construída durante a análise sintática,
por meio de ações semânticas embutidas em cada regra da gramática no `g-v1.y`.

A AST é uma representação em memória da estrutura lógica do programa. Ela
elimina detalhes sintáticos desnecessários (parênteses, ponto-e-vírgula) e
serve de entrada para as fases seguintes.

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

---

### ✅ Fase 4 — Tabela de Símbolos (`tabela.h` / `tabela.c`)

Implementada como uma **pilha de escopos**. Cada bloco `{ }` empilha uma nova
tabela ao ser aberto e a desempilha ao ser fechado. Isso permite que variáveis
de mesmo nome coexistam em blocos diferentes.

```
Ao entrar no bloco interno:        Ao sair do bloco interno:

  topo →  [ y : car  ]               topo →  [ x : int  ]
          [ x : int  ]                        (NULL)
          (NULL)
```

| Função                | O que faz                                              |
|-----------------------|--------------------------------------------------------|
| `iniciarPilha()`      | Cria a pilha vazia                                     |
| `empilharEscopo()`    | Abre um novo bloco                                     |
| `inserirSimbolo()`    | Declara uma variável no escopo atual                   |
| `buscarSimbolo()`     | Procura do escopo mais interno ao externo              |
| `desempilharEscopo()` | Fecha o bloco, removendo suas variáveis                |
| `liberarPilha()`      | Libera toda a memória                                  |

---

### ✅ Fase 5 — Analisador Semântico (`semantico.c`)

Percorre a AST recursivamente usando a tabela de símbolos para verificar se
o programa faz sentido — não só se é gramaticalmente correto, mas se respeita
as regras de tipo e escopo da linguagem G-V1.

Erros detectados:

| Situação | Mensagem de erro |
|---|---|
| Variável usada sem declaração | `Variavel nao declarada no escopo atual` |
| Variável declarada duas vezes no mesmo bloco | `Variavel redeclarada no mesmo escopo` |
| Atribuição com tipos diferentes (`car = int`) | `Atribuicao entre tipos incompativeis` |
| Aritmética com `car` | `Operadores aritmeticos exigem operandos do tipo int` |
| Lógica com `car` | `Operadores logicos exigem operandos do tipo int` |
| Relacional com tipos diferentes (`int < car`) | `Operadores relacionais exigem operandos de mesmo tipo` |
| Condição de `se`/`enquanto` do tipo `car` | `A condicao deve resultar em tipo int` |

Todos os erros são reportados no formato:
```
ERRO SEMANTICO: <mensagem> - linha: <número>
```

---

### ✅ Fase 6 — Geração de Código MIPS (`gerador.h` / `gerador.c`)

Se o programa passou por todas as fases sem erros, o gerador percorre a AST
mais uma vez e produz um arquivo `.s` com instruções em assembly **MIPS**.

O código gerado pode ser executado nos simuladores **SPIM** ou **MARS**.

#### Estratégia de memória

Variáveis locais são alocadas na **pilha de execução** do MIPS:
- Ao entrar em um bloco: `addi $sp, $sp, -N` (reserva N bytes)
- Variáveis são acessadas via `offset($fp)`
- Ao sair do bloco: `addi $sp, $sp, N` (devolve o espaço)

#### Syscalls utilizadas

| Operação | Syscall |
|---|---|
| Imprimir inteiro | 1 |
| Imprimir caractere | 11 |
| Imprimir string | 4 |
| Ler inteiro | 5 |
| Ler caractere | 12 |
| Encerrar programa | 10 |

#### Exemplo de saída

Para o programa:
```
principal
{
  { x : int; }
  { x = 5; escreva x; novalinha; }
}
```

O assembly gerado é:
```mips
    .text
    .globl main
main:
    move $fp, $sp
    addi $sp, $sp, -4
    li   $t0, 5
    sw   $t0, -4($fp)
    move $a0, $t0
    li   $v0, 1
    syscall
    li   $a0, 10
    li   $v0, 11
    syscall
    addi $sp, $sp, 4
    li   $v0, 10
    syscall
```

---

## Programas de teste

### Corretos (devem compilar e gerar assembly)

| Arquivo | Descrição |
|---|---|
| `fatorial.g` | Calcula o fatorial de um inteiro positivo |
| `Soma.g` | Soma os termos de uma progressão aritmética |
| `PA.g` | Calcula o N-ésimo termo de uma PA |
| `SeqOrdenada.g` | Verifica se uma sequência está em ordem crescente |
| `NotaEmConceito.g` | Converte nota numérica em conceito A/B/C/D |

### Erros semânticos intencionais (devem reportar erro e parar)

| Arquivo | Erro intencional |
|---|---|
| `fatorial-VariavelNaoDeclaradaLin19.g` | Variável `i` usada sem declaração |
| `fatorial-VariavelRedeclaradaNoMesmoEscopoLin9.g` | `fatorial` declarada duas vezes |
| `NotaEmConceito-Sub-expressoesDeTiposIncompativeisLin26.g` | `nota < '7'` mistura int e car |
| `NotaEmConceito-VariavelDeTipoIncopativelComExpressaoLin19.g` | `conceito = 4` atribui int a car |
| `NotaEmConceito-VariavelNaoDeclaradaNoEscopoAtual.g` | `conceito` usada fora do seu escopo |
