# Código MIPS gerado pelo compilador G-V1
    .text
    .globl main
main:
    # prologo: configura frame pointer
    move $fp, $sp
    # abre escopo: reserva 4 bytes na pilha
    addi $sp, $sp, -4
    # escreva string
    la   $a0, _str0
    li   $v0, 4
    syscall
    .data
_str0: .asciiz "digite o tamanho de uma sequencia de numeros inteiros - digite 0 para terminar."
    .text
    # leia int: quant
    li   $v0, 5
    syscall
    sw   $v0, -4($fp)
_label1:
    lw   $t0, -4($fp)
    li   $t1, 0
    sne  $t2, $t0, $t1
    # enquanto: pula se condicao falsa
    beqz $t2, _label2
    # abre escopo: reserva 12 bytes na pilha
    addi $sp, $sp, -12
    li   $t0, 118
    sb   $t0, -16($fp)
    li   $t0, 1
    sw   $t0, -8($fp)
    # leia int: valAtual
    li   $v0, 5
    syscall
    sw   $v0, -12($fp)
    # escreva string
    la   $a0, _str3
    li   $v0, 4
    syscall
    .data
_str3: .asciiz "digite uma sequencia de "
    .text
    lw   $t0, -4($fp)
    # escreva expressao
    move $a0, $t0
    li   $v0, 1
    syscall
    # escreva string
    la   $a0, _str4
    li   $v0, 4
    syscall
    .data
_str4: .asciiz " numeros inteiros separados entre si por um espaco"
    .text
_label5:
    lw   $t0, -8($fp)
    lw   $t1, -4($fp)
    slt  $t2, $t0, $t1
    # enquanto: pula se condicao falsa
    beqz $t2, _label6
    # abre escopo: reserva 4 bytes na pilha
    addi $sp, $sp, -4
    # leia int: proxVal
    li   $v0, 5
    syscall
    sw   $v0, -20($fp)
    lw   $t0, -12($fp)
    lw   $t1, -20($fp)
    slt  $t2, $t0, $t1
    # se-senao
    beqz $t2, _label7
    lw   $t0, -20($fp)
    sw   $t0, -12($fp)
    j    _label8
_label7:
    li   $t0, 102
    sb   $t0, -16($fp)
    lw   $t0, -4($fp)
    sw   $t0, -8($fp)
_label8:
    lw   $t0, -8($fp)
    li   $t1, 1
    add  $t2, $t0, $t1
    sw   $t2, -8($fp)
    # fecha escopo: libera 4 bytes da pilha
    addi $sp, $sp, 4
    j    _label5
_label6:
    lb   $t0, -16($fp)
    li   $t1, 118
    seq  $t2, $t0, $t1
    # se-senao
    beqz $t2, _label9
    # escreva string
    la   $a0, _str11
    li   $v0, 4
    syscall
    .data
_str11: .asciiz "ORDENADA"
    .text
    # novalinha
    li   $a0, 10
    li   $v0, 11
    syscall
    j    _label10
_label9:
    # escreva string
    la   $a0, _str12
    li   $v0, 4
    syscall
    .data
_str12: .asciiz "DESORDENADA"
    .text
    # novalinha
    li   $a0, 10
    li   $v0, 11
    syscall
_label10:
    # leia int: quant
    li   $v0, 5
    syscall
    sw   $v0, -4($fp)
    # fecha escopo: libera 12 bytes da pilha
    addi $sp, $sp, 12
    j    _label1
_label2:
    # fecha escopo: libera 4 bytes da pilha
    addi $sp, $sp, 4
    # epilogo: encerra o programa
    li   $v0, 10
    syscall
