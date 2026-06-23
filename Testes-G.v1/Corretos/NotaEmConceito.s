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
_str0: .asciiz "Digite um valor inteiro para a nota de um aluno"
    .text
    # novalinha
    li   $a0, 10
    li   $v0, 11
    syscall
    # leia int: nota
    li   $v0, 5
    syscall
    sw   $v0, -4($fp)
    lw   $t0, -4($fp)
    li   $t1, 6
    slt  $t2, $t0, $t1
    # se-senao
    beqz $t2, _label1
    # abre escopo: reserva 4 bytes na pilha
    addi $sp, $sp, -4
    li   $t0, 68
    sb   $t0, -8($fp)
    # escreva string
    la   $a0, _str3
    li   $v0, 4
    syscall
    .data
_str3: .asciiz "Conceito: "
    .text
    lb   $t0, -8($fp)
    # escreva expressao
    move $a0, $t0
    li   $v0, 11
    syscall
    # novalinha
    li   $a0, 10
    li   $v0, 11
    syscall
    # fecha escopo: libera 4 bytes da pilha
    addi $sp, $sp, 4
    j    _label2
_label1:
    lw   $t0, -4($fp)
    li   $t1, 7
    slt  $t2, $t0, $t1
    # se-senao
    beqz $t2, _label4
    # abre escopo: reserva 4 bytes na pilha
    addi $sp, $sp, -4
    li   $t0, 67
    sb   $t0, -8($fp)
    # escreva string
    la   $a0, _str6
    li   $v0, 4
    syscall
    .data
_str6: .asciiz "Conceito: "
    .text
    lb   $t0, -8($fp)
    # escreva expressao
    move $a0, $t0
    li   $v0, 11
    syscall
    # novalinha
    li   $a0, 10
    li   $v0, 11
    syscall
    # fecha escopo: libera 4 bytes da pilha
    addi $sp, $sp, 4
    j    _label5
_label4:
    lw   $t0, -4($fp)
    li   $t1, 9
    slt  $t2, $t0, $t1
    # se-senao
    beqz $t2, _label7
    # abre escopo: reserva 4 bytes na pilha
    addi $sp, $sp, -4
    li   $t0, 66
    sb   $t0, -8($fp)
    # escreva string
    la   $a0, _str9
    li   $v0, 4
    syscall
    .data
_str9: .asciiz "Conceito: "
    .text
    lb   $t0, -8($fp)
    # escreva expressao
    move $a0, $t0
    li   $v0, 11
    syscall
    # novalinha
    li   $a0, 10
    li   $v0, 11
    syscall
    # fecha escopo: libera 4 bytes da pilha
    addi $sp, $sp, 4
    j    _label8
_label7:
    # abre escopo: reserva 4 bytes na pilha
    addi $sp, $sp, -4
    li   $t0, 65
    sb   $t0, -8($fp)
    # escreva string
    la   $a0, _str10
    li   $v0, 4
    syscall
    .data
_str10: .asciiz "Conceito: "
    .text
    lb   $t0, -8($fp)
    # escreva expressao
    move $a0, $t0
    li   $v0, 11
    syscall
    # novalinha
    li   $a0, 10
    li   $v0, 11
    syscall
    # fecha escopo: libera 4 bytes da pilha
    addi $sp, $sp, 4
_label8:
_label5:
_label2:
    # fecha escopo: libera 4 bytes da pilha
    addi $sp, $sp, 4
    # epilogo: encerra o programa
    li   $v0, 10
    syscall
