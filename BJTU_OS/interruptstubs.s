.set IRQ_BASE,0x20
.section .text

.extern _ZN15InterruptManager15handleInterruptEhj
.global _ZN15InterruptManager22IgnoreInterruptRequestEv

.macro HandleInterruptRequest num
.global _ZN15InterruptManager26HandleInterruptRequest\num\()Ev
_ZN15InterruptManger26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    pushl $0
    jmp int_bottom
.endm

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x0C
HandleInterruptRequest 0x60



int_bottom:
    # pusha
    # push %ds
    # push %es
    # push %fs
    # push %gs

    pushl %ebp
    pushl %edi
    pushl %esi

    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    pushl %esp
    push (interruptnumber)

    call _ZN15InterruptManager15handleInterruptEhj

    #pop (interruptnumber)
    #popl %esp
    movl %eax,%esp

    popl %eax
    popl %ebx
    popl %ecx
    popl %edx

    popl %esi
    popl %edi
    popl %ebp

    # popl %gs
    # popl %fs
    # popl %es
    # popl %ds
    # popa

_ZN15InterruptManager22IgnoreInterruptRequestEv:
    iret

.data
    interruptnumber: .byte 0
