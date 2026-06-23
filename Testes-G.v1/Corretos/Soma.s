# Código MIPS gerado pelo compilador G-V1
    .text
    .globl main
main:
    # prologo: configura frame pointer
    move $fp, $sp
    # abre escopo: reserva 12 bytes na pilha
    addi $sp, $sp, -12
    # escreva string
    la   $a0, _str0
    li   $v0, 4
    syscall
    .data
_str0: .asciiz "Digite o valor do termo inical da progressao aritmetica"
    .text
    # novalinha
    li   $a0, 10
    li   $v0, 11
    syscall
    # leia int: a1
    li   $v0, 5
    syscall
    sw   $v0, -4($fp)
    # escreva string
    la   $a0, _str1
    li   $v0, 4
    syscall
    .data
_str1: .asciiz "Digite  o número de elementos da progressao artimetica"
    .text
    # novalinha
    li   $a0, 10
    li   $v0, 11
    syscall
    # leia int: n
    li   $v0, 5
    syscall
    sw   $v0, -12($fp)
    # escreva string
    la   $a0, _str2
    li   $v0, 4
    syscall
    .data
_str2: .asciiz "Digite a razao da progressao"
    .text
    # novalinha
    li   $a0, 10
    li   $v0, 11
    syscall
    # leia int: r
    li   $v0, 5
    syscall
    sw   $v0, -8($fp)
    # abre escopo: reserva 12 bytes na pilha
    addi $sp, $sp, -12
    lw   $t0, -4($fp)
    sw   $t0, -16($fp)
    li   $t0, 1
    sw   $t0, -20($fp)
    lw   $t0, -4($fp)
    sw   $t0, -24($fp)
_label3:
    lw   $t0, -20($fp)
    lw   $t1, -12($fp)
    slt  $t2, $t0, $t1
    # enquanto: pula se condicao falsa
    beqz $t2, _label4
    lw   $t0, -24($fp)
    lw   $t1, -8($fp)
    add  $t2, $t0, $t1
    sw   $t2, -24($fp)
    lw   $t0, -20($fp)
    li   $t1, 1
    add  $t2, $t0, $t1
    sw   $t2, -20($fp)
    lw   $t0, -16($fp)
    lw   $t1, -24($fp)
    add  $t2, $t0, $t1
    sw   $t2, -16($fp)
    j    _label3
_label4:
    # escreva string
    la   $a0, _str5
    li   $v0, 4
    syscall
    .data
_str5: .asciiz "O valor da soma da progressao aritmetica e: "
    .text
    lw   $t0, -16($fp)
    # escreva expressao
    move $a0, $t0
    li   $v0, 1
    syscall
    # novalinha
    li   $a0, 10
    li   $v0, 11
    syscall
    # fecha escopo: libera 12 bytes da pilha
    addi $sp, $sp, 12
    # fecha escopo: libera 12 bytes da pilha
    addi $sp, $sp, 12
    # epilogo: encerra o programa
    li   $v0, 10
    syscall
