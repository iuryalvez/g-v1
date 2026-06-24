/*
 * tabela.h — Tabela de Símbolos (pilha de escopos) para a linguagem G-V2
 *
 * Imagine a tabela de símbolos como uma pilha de folhas de papel.
 * Cada vez que o programa entra em um bloco, uma folha nova é colocada
 * no topo da pilha. Nessa folha ficam anotados os nomes (variáveis,
 * parâmetros) declarados naquele bloco. Quando o bloco termina, a folha
 * do topo é jogada fora — e os nomes declarados ali deixam de existir.
 *
 * Diferente da versão usada em G-V1, a pilha de G-V2 já nasce com uma
 * folha na base, representando o ESCOPO GLOBAL (escopo 0). É nessa folha
 * que ficam as variáveis globais e os nomes de TODAS as funções do
 * programa — variáveis globais e funções compartilham o mesmo espaço de
 * nomes, exatamente como descrito no enunciado do trabalho.
 *
 * Cada folha da pilha sabe a que profundidade de aninhamento ela
 * corresponde (campo 'escopo' da struct Tabela). Essa numeração é a
 * mesma usada nos slides de Geração de Código (Acesso a variáveis em
 * blocos aninhados): escopo 0 referencia variáveis globais (a partir de
 * $s1) e escopo > 0 referencia variáveis locais/parâmetros (a partir de
 * $fp). Guardar esse número agora poupa trabalho na fase de geração de
 * código.
 *
 * Exemplo de pilha durante a análise do corpo de uma função f(a, b):
 *
 *   topo →  [ a:1, c:2 ]   ← escopo 2: bloco interno de f
 *           [ a:0, b:1, f(a,b) ]  ← escopo 1: parâmetros de f, bloco
 *                                    mais externo de f (compartilham
 *                                    o mesmo escopo: ver regra do
 *                                    enunciado sobre redeclaração)
 *   base →  [ x:0, y:1, f, g, ... ]  ← escopo 0: globais e funções
 */

#ifndef TABELA_H
#define TABELA_H

/*
 * TipoVar — tipos possíveis para uma variável ou parâmetro em G-V2.
 *
 * Um único campo já identifica o tipo básico (int/car) e se o nome se
 * refere a um vetor desse tipo básico. Isso simplifica a checagem de
 * tipos na análise semântica: comparar tipos é comparar este enum.
 */
typedef enum {
    TIPO_INT,        /* variável/parâmetro escalar do tipo int          */
    TIPO_CAR,        /* variável/parâmetro escalar do tipo car          */
    TIPO_VETOR_INT,  /* vetor de int                                    */
    TIPO_VETOR_CAR   /* vetor de car                                    */
} TipoVar;

/*
 * CategoriaSimbolo — para que serve o nome guardado na entrada.
 *
 * A análise semântica precisa diferenciar, por exemplo, "x" usado como
 * variável de "x" usado (erradamente) como se fosse uma função.
 */
typedef enum {
    CATEGORIA_VARIAVEL,   /* variável global ou local                   */
    CATEGORIA_PARAMETRO,  /* parâmetro formal de uma função             */
    CATEGORIA_FUNCAO      /* nome de função                             */
} CategoriaSimbolo;

/*
 * ParametroAssinatura — descreve UM parâmetro formal de uma função,
 * apenas para fins de checagem de chamada (tipo e, por completude,
 * o lexema do parâmetro — útil em mensagens de erro).
 *
 * Esta lista NÃO é visível pela busca normal (buscarSimbolo). Ela fica
 * "pendurada" na entrada da função e só é consultada explicitamente
 * quando a análise semântica confere os argumentos de uma chamada.
 * Isso resolve o "quase-dilema" do enunciado: os parâmetros em si só
 * passam a existir como símbolos visíveis quando a função entra em
 * análise (via inserirParametro, no escopo do corpo da função); fora
 * dali, só a assinatura (tipo) é acessível, não o lexema como símbolo
 * utilizável em uma expressão.
 */
typedef struct ParametroAssinatura {
    char *nome;                        /* lexema do parâmetro formal    */
    TipoVar tipo;                      /* tipo do parâmetro formal      */
    struct ParametroAssinatura *prox;  /* próximo parâmetro da lista    */
} ParametroAssinatura;

/*
 * EntradaTabela — representa um nome declarado: variável, parâmetro
 * ou função.
 *
 * Os campos 'numParametros' e 'assinatura' só têm sentido quando
 * categoria == CATEGORIA_FUNCAO; nos demais casos ficam zerados/NULL.
 */
typedef struct EntradaTabela {
    char *nome;                  /* nome (lexema) do símbolo                */
    CategoriaSimbolo categoria;  /* variável, parâmetro ou função           */
    TipoVar tipo;                /* tipo da variável/parâmetro, ou tipo de
                                     RETORNO, no caso de uma função         */
    int escopo;                  /* 0 = global; > 0 = profundidade do bloco */
    int posicao;                 /* posição na lista de declaração (usada
                                     na geração de código); não usado p/
                                     funções                               */
    int linha;                   /* linha de declaração (para erros)       */

    /* --- usados apenas quando categoria == CATEGORIA_FUNCAO --- */
    int numParametros;                 /* quantidade de parâmetros formais */
    ParametroAssinatura *assinatura;   /* lista de tipos dos parâmetros    */

    struct EntradaTabela *prox;  /* próximo símbolo no mesmo escopo         */
} EntradaTabela;

/*
 * Tabela — representa uma "folha" da pilha: o escopo de um bloco.
 *
 * Cada tabela é uma lista encadeada de símbolos declarados naquele
 * escopo, mais um ponteiro para o escopo anterior (embaixo na pilha) e
 * o número que identifica a profundidade deste escopo.
 */
typedef struct Tabela {
    EntradaTabela *entradas;  /* lista de símbolos deste escopo           */
    struct Tabela *anterior;  /* escopo anterior (embaixo na pilha)       */
    int escopo;                /* profundidade: 0 = global                */
} Tabela;

/*
 * PilhaTabela — a pilha em si.
 *
 * O campo 'topo' aponta para o escopo mais recente (o bloco mais
 * interno que está sendo analisado no momento). Após iniciarPilha(),
 * 'topo' já aponta para uma tabela vazia de escopo 0 (o escopo global).
 */
typedef struct {
    Tabela *topo;  /* escopo atual (topo da pilha)                       */
} PilhaTabela;

/* --- Operações sobre a pilha de tabelas --- */

/*
 * iniciarPilha — cria a pilha e já abre o escopo global (escopo 0).
 * Deve ser chamada uma única vez antes de iniciar a análise semântica.
 */
PilhaTabela *iniciarPilha();

/*
 * empilharEscopo — cria um novo escopo e o coloca no topo da pilha.
 * O novo escopo recebe profundidade igual a (escopo do topo atual + 1).
 *
 * Chamada ao entrar em um bloco com declarações (corpo de função,
 * bloco { } aninhado, etc).
 */
void empilharEscopo(PilhaTabela *pilha);

/*
 * desempilharEscopo — remove o escopo do topo da pilha.
 *
 * Chamada ao sair de um bloco, liberando todos os símbolos locais a
 * ele. NÃO pode ser usada para remover o escopo global (escopo 0):
 * nesse caso o programa é interrompido com uma mensagem de erro, pois
 * isso indicaria um bug no compilador, não um erro do programa fonte.
 */
void desempilharEscopo(PilhaTabela *pilha);

/*
 * buscarSimbolo — procura um nome em todos os escopos, do topo à base.
 *
 * Essa busca respeita o sombreamento de nomes: se um nome x existe no
 * escopo interno e no externo, o do escopo interno é retornado.
 *
 * Retorna um ponteiro para a entrada encontrada, ou NULL se o nome não
 * estiver declarado em nenhum escopo visível.
 */
EntradaTabela *buscarSimbolo(PilhaTabela *pilha, const char *nome);

/*
 * inserirVariavel — declara uma variável (global ou local) no escopo
 * atual (topo da pilha).
 *
 * 'posicao' é a posição da variável dentro da lista de declaração onde
 * ela apareceu (1, 2, 3, ...) — informação usada futuramente na
 * geração de código para calcular deslocamentos na pilha/memória.
 *
 * Retorna um ponteiro para a entrada criada, ou NULL se já existir um
 * símbolo com este nome NO ESCOPO ATUAL (erro semântico de
 * redeclaração).
 */
EntradaTabela *inserirVariavel(PilhaTabela *pilha, const char *nome,
                                TipoVar tipo, int posicao, int linha);

/*
 * inserirParametro — declara um parâmetro formal no escopo atual.
 *
 * Deve ser chamada depois que o escopo do corpo da função já foi
 * aberto com empilharEscopo, e ANTES de processar as declarações de
 * variáveis locais do bloco mais externo da função — assim parâmetros
 * e variáveis locais desse bloco competem pelo mesmo escopo, e a
 * tentativa de declarar uma variável local com o mesmo nome de um
 * parâmetro é pega automaticamente como redeclaração (regra do
 * enunciado).
 *
 * Retorna um ponteiro para a entrada criada, ou NULL em caso de
 * redeclaração no escopo atual.
 */
EntradaTabela *inserirParametro(PilhaTabela *pilha, const char *nome,
                                 TipoVar tipo, int posicao, int linha);

/*
 * inserirFuncao — declara uma função no escopo atual.
 *
 * Deve ser chamada estando o topo da pilha no escopo global (escopo 0),
 * já que em G-V2 todas as funções são globais. A tabela em si não
 * fiscaliza isso — quem decide quando chamar é a análise semântica.
 *
 * Após inserir a função, use adicionarParametroAssinatura() uma vez
 * para cada parâmetro formal, na ordem em que aparecem na declaração,
 * para montar a assinatura usada na checagem de chamadas.
 *
 * Retorna um ponteiro para a entrada criada, ou NULL em caso de
 * redeclaração no escopo atual (nome de função repetido, ou conflito
 * com uma variável global de mesmo nome).
 */
EntradaTabela *inserirFuncao(PilhaTabela *pilha, const char *nome,
                              TipoVar tipoRetorno, int linha);

/*
 * adicionarParametroAssinatura — anexa a descrição de um parâmetro
 * formal à assinatura de uma função já inserida com inserirFuncao.
 *
 * Os parâmetros devem ser adicionados na MESMA ORDEM em que aparecem
 * na declaração da função, pois a checagem de uma chamada compara essa
 * lista posicionalmente com os argumentos passados.
 */
void adicionarParametroAssinatura(EntradaTabela *funcao, const char *nome,
                                   TipoVar tipo);

/*
 * liberarPilha — libera toda a memória da pilha, incluindo o escopo
 * global. Chamada ao final da compilação.
 */
void liberarPilha(PilhaTabela *pilha);

/*
 * imprimirPilha — imprime o conteúdo de todos os escopos da pilha,
 * do topo à base. Útil para depurar a tabela de símbolos de forma
 * isolada, antes de integrá-la à análise semântica.
 */
void imprimirPilha(PilhaTabela *pilha);

#endif /* TABELA_H */
