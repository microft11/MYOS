.set IRQ_BASE,0x20
.section .text

.extern _ZN15InterruptManger15handleInterruptEhj
.global _ZN15InterruptManger22IgnoreInterruptRequestEv

.macro HandleInterruptRequest num
.global _ZN15InterruptManger26HandleInterruptRequest\num\()Ev
_ZN15InterruptManger26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    jmp int_bottom
.endm

HandleInterruptRequest 0x00



int_bottom:
    pusha
    push %ds
    push %es
    push %fs
    push %gs

    pushl %esp
    push (interruptnumber)

    call _ZN15InterruptManger15handleInterruptEhj

    #pop (interruptnumber)
    #popl %esp
    movl %eax,%esp

    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

_ZN15InterruptManger22IgnoreInterruptRequestEv:
    iret

.data
    interruptnumber: .byte 0
