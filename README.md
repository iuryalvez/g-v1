# Compilador G-V1

## Descrição
[cite_start]Implementação de um compilador didático para a linguagem G-V1. [cite_start]O projeto utiliza Flex para análise léxica e Bison para análise sintática.

## Dependências
[cite_start]O ambiente de avaliação utilizará as seguintes versões:
- [cite_start]GCC/G++ versão 14
- [cite_start]Flex versão 2.6.4
- [cite_start]Bison versão 3.8.2

## Estrutura do Projeto
- `g-v1.l`: Especificação do analisador léxico.
- `testes/teste.gv1`: Arquivo de teste da linguagem.
- [cite_start]`Makefile`: Script de automação de compilação.

## Execução

[cite_start]Gere o executável do compilador através do comando:
```bash
make