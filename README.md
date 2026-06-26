# Compilador G-V2

Implementação de um compilador didático para a linguagem **G-V2**, desenvolvido
para a disciplina de Compiladores da Universidade Federal de Goiás. G-V2 é uma
extensão de **G-V1** (já entregue como Trabalho 1) que adiciona **funções** e
**vetores** (de `int` ou de `car`) à linguagem.

> O repositório se chama `g-v1` por ter nascido no Trabalho 1, mas todo o
> conteúdo aqui (gramática, fases do compilador, executável `g-v2`) já é a
> versão G-V2.

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

Os testes que usam o simulador MIPS (ver "Programas de teste") exigem **Java**
(usado para rodar `Testes_G-V2/Mars4_5.jar`, o MARS — MIPS Assembler and
Runtime Simulator).

---

## Estrutura do Projeto

```
g-v2/
├── g-v2.l         # Analisador léxico (Flex)
├── g-v2.y         # Analisador sintático + construção da AST (Bison)
├── ast.h          # Definição dos tipos e funções da AST
├── ast.c          # Implementação da AST
├── tabela.h       # Definição da tabela de símbolos (pilha de escopos)
├── tabela.c       # Implementação da tabela de símbolos
├── semantico.c    # Analisador semântico (percurso na AST)
├── gerador.h      # Definição do gerador de código MIPS
├── gerador.c      # Implementação do gerador de código MIPS
├── Makefile       # Script de compilação
└── Testes_G-V2/
    ├── Corretos/         # Programas sem erros
    ├── ErrosSemanticos/  # Programas com erros semânticos intencionais
    └── Mars4_5.jar       # Simulador MIPS, para rodar os .s gerados
```

---

## Como compilar e executar

```bash
# Compila tudo e gera o executável g-v2
make

# Executa o compilador em um arquivo de entrada
./g-v2 programa.txt

# Remove arquivos gerados (objetos, executável, assembly)
make clean

# Executa a bateria de testes
make test
```

Se o programa estiver correto, o compilador:
1. Imprime `Analise sintatica concluida com sucesso.`
2. Imprime `Analise semantica concluida com sucesso.`
3. Gera um arquivo `.s` com o assembly MIPS (mesmo nome do arquivo de entrada)
4. Imprime `Codigo MIPS gerado em: <arquivo>.s`

Para rodar o `.s` gerado no simulador:
```bash
java -jar Testes_G-V2/Mars4_5.jar nc programa.s
```

> **Observação sobre `leia` e entrada não-interativa:** ao alimentar o MARS
> com entrada via pipe (`echo "..." | java -jar ...`), ele só lê corretamente
> se cada valor estiver em uma **linha separada** — números separados por
> espaço na mesma linha não funcionam em modo não-interativo (`nc`). Use
> `printf "1\n2\n3\n" | java -jar ...` em vez de `echo "1 2 3" | ...`.

---

## Fases implementadas

### ✅ Fase 1 — Analisador Léxico (`g-v2.l`)

Gerado automaticamente pelo **Flex**. Lê o arquivo de entrada caractere por
caractere e agrupa os símbolos em tokens — a menor unidade com significado
na linguagem (palavras reservadas, identificadores, operadores, constantes).

Tokens reconhecidos:
- Palavras reservadas: `principal`, `int`, `car`, `leia`, `escreva`,
  `novalinha`, `se`, `entao`, `senao`, `fimse`, `enquanto`, `global`,
  `funcao`, `retorne`
- Identificadores, constantes inteiras, constantes de caractere e strings
- Operadores aritméticos, relacionais e lógicos
- Os novos delimitadores de G-V2: `[` e `]` (seção de variáveis globais,
  seção de declaração de funções, declaração e acesso a vetores)
- Comentários `/* ... */` (descartados, podem ocupar várias linhas)

Erros léxicos reportados:
```
ERRO: CARACTERE INVALIDO <linha>
ERRO: COMENTARIO NAO TERMINA <linha>
ERRO: CADEIA DE CARACTERES OCUPA MAIS DE UMA LINHA <linha>
```

---

### ✅ Fase 2 — Analisador Sintático (`g-v2.y`)

Gerado automaticamente pelo **Bison** (método LALR(1)). Verifica se a sequência
de tokens respeita as regras gramaticais da linguagem G-V2 — que muda em
relação a G-V1 principalmente na estrutura inicial do programa:

```
global [ <variáveis globais> ]
funcao [ <nome>(<parâmetros>): <tipo> <bloco> ... ]
principal <bloco>
```

Dentro de um bloco, a seção de variáveis locais agora é delimitada por `[ ]`
(em G-V1 era `{ }`); o bloco de comandos continua com `{ }`.

Erros sintáticos são reportados com o formato:
```
ERRO: <mensagem> proximo a '<token>' - linha: <número>
```

---

### ✅ Fase 3 — Geração da AST (`ast.h` / `ast.c`)

A **Árvore Sintática Abstrata (AST)** é construída durante a análise sintática,
por meio de ações semânticas embutidas em cada regra da gramática no `g-v2.y`.

Cada nó da AST é representado pela struct `No`, que em G-V2 ganhou um quarto
filho (necessário para representar uma declaração de função: identificador,
tipo de retorno, parâmetros e bloco, tudo no mesmo nó):

```c
typedef struct No {
    TipoNo tipo;      // que construção da linguagem este nó representa
    int linha;        // linha no fonte (para mensagens de erro)
    char *valor;      // texto léxico (apenas em nós folha)
    struct No *filho1;
    struct No *filho2;
    struct No *filho3;
    struct No *filho4; // novo em G-V2: usado por AST_FUNCAO
} No;
```

Novos tipos de nó em relação a G-V1: `AST_GLOBAIS`, `AST_LISTA_FUNC`,
`AST_FUNCAO`, `AST_LISTA_PARAM`, `AST_PARAMETRO`, `AST_VETOR_DECL`,
`AST_VETOR_ACESSO`, `AST_CHAMADA_FUNC`, `AST_LISTA_EXPR`, `AST_CMD_RETORNE`.

---

### ✅ Fase 4 — Tabela de Símbolos (`tabela.h` / `tabela.c`)

Implementada como uma **pilha de escopos**, igual a G-V1, mas com três
diferenças importantes para suportar funções e vetores:

- A pilha **nasce com o escopo global já aberto** (escopo 0) — variáveis
  globais e nomes de função vivem ali do início ao fim da compilação,
  diferente de G-V1, onde a pilha nascia vazia.
- Cada entrada guarda uma **categoria** (`CATEGORIA_VARIAVEL`,
  `CATEGORIA_PARAMETRO` ou `CATEGORIA_FUNCAO`), permitindo distinguir, por
  exemplo, um identificador usado como variável de um usado (erradamente)
  como função.
- Funções guardam uma **assinatura** própria (lista de tipos dos parâmetros
  formais), usada na checagem de chamadas — independente dos parâmetros
  em si só existirem como símbolos buscáveis dentro do corpo da função.

```
Ao entrar no corpo de uma função f(a, b), com um bloco aninhado:

  topo →  [ c : int ]                  ← escopo 2: bloco aninhado em f
          [ a : int, b : car ]         ← escopo 1: parâmetros + bloco
                                           mais externo de f (MESMO escopo)
  base →  [ x : int, f, g, ... ]       ← escopo 0: globais e funções
```

| Função                          | O que faz                                                  |
|----------------------------------|-------------------------------------------------------------|
| `iniciarPilha()`                 | Cria a pilha já com o escopo global aberto                 |
| `empilharEscopo()`               | Abre um novo bloco                                          |
| `inserirVariavel()`              | Declara uma variável (global ou local) no escopo atual     |
| `inserirParametro()`             | Declara um parâmetro formal no escopo do corpo da função   |
| `inserirFuncao()`                | Declara uma função no escopo global                        |
| `adicionarParametroAssinatura()` | Anexa um parâmetro à assinatura de uma função já inserida  |
| `buscarSimbolo()`                | Procura do escopo mais interno ao mais externo               |
| `desempilharEscopo()`            | Fecha o bloco (trava se for o escopo global)                  |
| `liberarPilha()`                 | Libera toda a memória, inclusive o escopo global             |
| `imprimirPilha()`                | Depuração: imprime todos os escopos e símbolos               |

---

### ✅ Fase 5 — Analisador Semântico (`semantico.c`)

Percorre a AST recursivamente, usando a tabela de símbolos para verificar se
o programa respeita as regras de tipo e escopo de G-V2. As funções são
processadas em **duas passadas**: a primeira registra todos os cabeçalhos
(nome, tipo de retorno, assinatura) antes de entrar em qualquer corpo — isso
permite uma função chamar outra declarada depois dela, inclusive recursão
mútua, sem precisar de protótipo.

Erros detectados (além de todos os já existentes em G-V1):

| Situação | Mensagem de erro |
|---|---|
| Variável/parâmetro/vetor redeclarado no mesmo escopo | `Variavel redeclarada no mesmo escopo` |
| Parâmetro redeclarado na mesma função | `Parametro redeclarado na mesma funcao` |
| Função redeclarada ou conflitando com símbolo global | `Funcao redeclarada ou em conflito com simbolo global` |
| Identificador de função usado como variável/vetor | `Identificador de funcao usado como variavel` / `...como vetor` |
| Indexação de algo que não é vetor | `Tentativa de indexar uma variavel que nao e vetor` |
| Índice de vetor que não é `int` | `Indice de vetor deve ser do tipo int` |
| Chamada de função não declarada | `Funcao nao declarada` |
| Identificador chamado como função, mas não é função | `Identificador usado como funcao nao e uma funcao` |
| Número de argumentos diferente do número de parâmetros | `Numero de argumentos nao corresponde ao numero de parametros` |
| Tipo de argumento incompatível com o parâmetro formal | `Tipo de argumento incompativel com o parametro formal` |
| `retorne` usado fora de uma função | `Comando 'retorne' usado fora de uma funcao` |
| Tipo de `retorne` incompatível com o tipo da função | `Tipo de retorno incompativel com o tipo da funcao` |
| Atribuição entre tipos incompatíveis (inclusive vetor × escalar, ou vetores de tipos básicos diferentes) | `Atribuicao entre tipos incompativeis` |

Todos os erros são reportados no formato:
```
ERRO SEMANTICO: <mensagem> - linha: <número>
```

---

### ✅ Fase 6 — Geração de Código MIPS (`gerador.h` / `gerador.c`)

Se o programa passou por todas as fases sem erros, o gerador percorre a AST
mais uma vez e produz um arquivo `.s` com instruções em assembly **MIPS**,
executável nos simuladores **SPIM** ou **MARS**.

#### Estratégia de memória

- **Variáveis globais** vivem num bloco fixo reservado em `.data`
  (`_globais`), acessado a partir do registrador `$s1` (carregado uma única
  vez, no início do programa).
- **Variáveis locais e parâmetros** são acessados via `$fp + deslocamento`,
  como em G-V1.
- **Vetores** (locais, globais ou recebidos por parâmetro) são representados
  de forma uniforme: o "slot" da variável guarda sempre um **ponteiro** de 4
  bytes para os dados. Um vetor declarado (local ou global) tem, além do
  slot, uma área de dados própria cujo endereço é calculado e guardado no
  slot no momento da declaração; um vetor recebido por parâmetro já chega
  com esse ponteiro preenchido pelo chamador (passagem por referência). Por
  isso, `vetorB = vetorA` é só uma cópia de 4 bytes (o ponteiro) entre slots.

#### Convenção de chamada de função

Cada função vira um label `_func_NOME`. O chamador empilha os argumentos do
**último para o primeiro**, de modo que o parâmetro de posição `p` sempre
fica em `$fp + 8 + 4*(p-1)`, independente de quantos parâmetros a função
tem. A própria função salva `$ra`/`$fp` antigo, monta seu frame, e ao final
(`retorne` ou fim do corpo) desfaz tudo — inclusive os bytes dos argumentos
empilhados pelo chamador ("callee-pop": o chamador não limpa nada depois do
`jal`).

Como os registradores temporários `$t0`–`$t9` não são preservados através de
uma chamada de função, o gerador salva na pilha qualquer temporário que uma
expressão ainda precise depois de uma chamada (ex: o "1" em `1 + f(2)`) e
restaura depois do `jal`.

#### Syscalls utilizadas

| Operação | Syscall |
|---|---|
| Imprimir inteiro | 1 |
| Imprimir caractere | 11 |
| Imprimir string | 4 |
| Ler inteiro | 5 |
| Ler caractere | 12 |
| Encerrar programa | 10 |

---

## Programas de teste

### Corretos (`Testes_G-V2/Corretos/`)

Programas que exercitam funções, vetores, recursão e variáveis globais —
incluem variações (`V2`, `V3`) testando vetores de tipos diferentes,
passagem por referência e sombreamento de variável local sobre global.

### Erros semânticos intencionais (`Testes_G-V2/ErrosSemanticos/`)

| Arquivo | Erro intencional |
|---|---|
| `fatorial-ChamadaComMaisParametrosLin32.txt` | Chamada de função com número de argumentos errado |
| `fatorial-variávelComMesmoNomeEescopoDeParametroLin9.txt` | Variável com mesmo nome de parâmetro no mesmo escopo |
| `selectionSortPassagemDeIntParaArgumentoVetLin40.txt` | Passagem de `int` onde se espera vetor |
| `selectionSortPassagemDeVetorComtipodiferenteDoArgumentoLin40.txt` | Vetor de tipo básico diferente do parâmetro |
| `somatorioVetorLin12ErroSomaVetorCompletocomVariavelNaoVetor.txt` | Indexação de variável que não é vetor |
| `somatorioVetorLin14ErroSomaVetorSemIndices.txt` | Uso de vetor sem índice onde se espera um elemento |
| `somatorioVetorLin8VarSomaNãoDeclaradaNoescopo.txt` | Variável usada fora do escopo onde foi declarada |
