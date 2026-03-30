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