# Código MIPS gerado pelo compilador G-V1
    .text
    .globl main
main:
    # prologo: configura frame pointer
    move $fp, $sp
    # abre escopo: reserva 12 bytes na pilha
    addi $sp, $sp, -12
    li   $t0, 1
    sub  $t1, $zero, $t0
    sw   $t1, -12($fp)
_label0:
    lw   $t0, -12($fp)
    li   $t1, 0
    slt  $t2, $t1, $t0
    xori $t2, $t2, 1
    # enquanto: pula se condicao falsa
    beqz $t2, _label1
    # escreva string
    la   $a0, _str2
    li   $v0, 4
    syscall
    .data
_str2: .asciiz "Digite um numero inteiro nao negativo"
    .text
    # novalinha
    li   $a0, 10
    li   $v0, 11
    syscall
    # leia int: n
    li   $v0, 5
    syscall
    sw   $v0, -12($fp)
    j    _label0
_label1:
    lw   $t0, -12($fp)
    sw   $t0, -4($fp)
    lw   $t0, -12($fp)
    li   $t1, 1
    sub  $t2, $t0, $t1
    sw   $t2, -8($fp)
_label3:
    lw   $t0, -8($fp)
    li   $t1, 1
    slt  $t2, $t1, $t0
    # enquanto: pula se condicao falsa
    beqz $t2, _label4
    lw   $t0, -4($fp)
    lw   $t1, -8($fp)
    mul  $t2, $t0, $t1
    sw   $t2, -4($fp)
    lw   $t0, -8($fp)
    li   $t1, 1
    sub  $t2, $t0, $t1
    sw   $t2, -8($fp)
    j    _label3
_label4:
    # escreva string
    la   $a0, _str5
    li   $v0, 4
    syscall
    .data
_str5: .asciiz "O fatorial de "
    .text
    lw   $t0, -12($fp)
    # escreva expressao
    move $a0, $t0
    li   $v0, 1
    syscall
    # escreva string
    la   $a0, _str6
    li   $v0, 4
    syscall
    .data
_str6: .asciiz " e: "
    .text
    lw   $t0, -4($fp)
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
    # epilogo: encerra o programa
    li   $v0, 10
    syscall
