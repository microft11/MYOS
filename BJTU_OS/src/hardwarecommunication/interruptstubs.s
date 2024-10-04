.set IRQ_BASE,0x20
.section .text

.extern _ZN4myos21hardwarecommunication16InterruptManager15handleInterruptEhj
.global _ZN4myos21hardwarecommunication16InterruptManager22IgnoreInterruptRequestEv

.macro HandleInterruptRequest num
.global _ZN4myos21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN4myos21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    pushl $0
    jmp int_bottom
.endm

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x0C
HandleInterruptRequest 0x60



int_bottom:
    
    # save resigster
    # pusha
    # pushl %ds
    # pushl %es
    # pushl %fs
    # pushl %gs
    pushl %ebp
    pushl %edi
    pushl %esi

    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    # load ring 0 segment register
    # cld
    # $0x10, %eax
    # %eax, %eds
    # %eax, %ees

    # call C++ Handler
    pushl %esp
    push (interruptnumber)
    call _ZN4myos21hardwarecommunication16InterruptManager15handleInterruptEhj
    # add %esp, 6
    mov %eax, %esp # switch the stack
                   # %eax always contains return value 
    # restore registers
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

    add $4, %esp

_ZN4myos21hardwarecommunication16InterruptManager22IgnoreInterruptRequestEv:
    iret

.data
    interruptnumber: .byte 0
