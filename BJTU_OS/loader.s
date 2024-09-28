.set MAGIC,  0x1badb002  # magic number 让人相信他是个内核
.set FLAGS, (1<<0 | 1<<1)
.set CHECKNUM, -(MAGIC+FLAGS) # 校验和

.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKNUM

.section .text
.extern kernelMain
.extern callConstructors
.global loader

loader:
    mov $kernel_stack, %esp
    call callConstructors
    push %eax
    push %ebx
    call kernelMain

_stop:
    cli
    hlt
    jmp _stop


.section .bss
.space 2*1024*1024
kernel_stack:
