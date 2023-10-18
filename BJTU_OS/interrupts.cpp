#include "interrupts.h"
void printf(const int8_t*);
InterruptManger::GateDescriptor InterruptManger::interruptDescriptorTable[256];

InterruptManger::InterruptManger(GlobalDescriptorTable *gdt)
    : picMasterCommand(0x20), picMasterData(0x21), picSlaveCommand(0xA0), picSlaveData(0xA1)
{
    uint16_t CodeSegment = gdt->CodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;
    //定义了中断门描述符的类型

    for (uint16_t i = 0; i < 256; i++)
    {
        SetInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
    }
    // 通过 for 循环，遍历256个中断号（0-255），对每个中断号都调用 SetInterruptDescriptorTableEntry 函数来设置
    // IDT表的中断门描述符。
    // 每个中断门描述符的中断处理函数都被设置为 IgnoreInterruptRequest，这个函数用于忽略中断请求，相当于占位符。

    SetInterruptDescriptorTableEntry(0x20, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);

    // 初始化可编程控制器
    picMasterCommand.Write(0x11);
    picSlaveCommand.Write(0x11);
    // 中断向量开始地址
    picMasterData.Write(0x20);
    picSlaveData.Write(0x28);
    // 输出数据
    picMasterData.Write(0x04);
    picSlaveData.Write(0x02);

    picMasterData.Write(0x01);
    picMasterData.Write(0x01);
    // 中断屏蔽码0闭1开
    picMasterData.Write(0x00);
    picSlaveData.Write(0x00);
    /*发送初始化命令 0x11 给主PIC和从PIC，用于重新初始化这两个PIC。
    配置主PIC和从PIC的中断向量起始地址。主PIC的起始地址为 0x20，从PIC的起始地址为 0x28。
    通过 picMasterData.Write(0x04) 和 picSlaveData.Write(0x02) 来告诉PIC它们的中断控制关系（主从关系）。
    最后，通过 picMasterData.Write(0x00) 和 picSlaveData.Write(0x00) 来设置中断屏蔽码，将所有中断都关闭。*/

    InterruptsDescriptorTablePointer idt;  //存储 IDT 表的指针和大小信息的数据结构
    idt.size = 256 *sizeof(GateDescriptor)-1;
    idt.base = (uint32_t)interruptDescriptorTable;

    __asm__ volatile("lidt %0": :"m" (idt));
    /*内联汇编指令 lidt：
         __asm__ volatile 是内联汇编的语法，用于在C++代码中插入汇编指令。
        "lidt %0" 是 lidt 汇编指令的操作数，%0 表示要替换的第一个操作数。这里，%0 会被替换为 idt 结构体的地址。
        冒号后面的内容是汇编指令的输入和输出参数。在这种情况下，没有输出参数，只有一个输入参数，即 idt 结构体的地址。
        "m" 表示将 idt 结构体作为内存操作数传递给 lidt 指令*/
}

InterruptManger::~InterruptManger()
{
}


void InterruptManger::Activate(){
    __asm__ volatile("sti");
}

void InterruptManger::Deactive(){
    __asm__ volatile("cli");
}


void InterruptManger::SetInterruptDescriptorTableEntry(
    uint8_t interruptNumber,            // 中断号
    uint16_t codeSegmentSelectorOffset, // 段选择符偏移
    void (*handler)(),                  // interrupt处理函数
    uint8_t DescriptorPrivilegeLevel,
    uint8_t DescriptorType)
{
    const uint8_t IDT_DESC_PRESENT = 0x80;
    interruptDescriptorTable[interruptNumber].handlerAddressLowBits=((uint32_t)handler)&0xFFFF;
    interruptDescriptorTable[interruptNumber].handlerAddressHighBits=((uint32_t)handler>>16)&0xFFFF;
    interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector=codeSegmentSelectorOffset;
    interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT|DescriptorType|(DescriptorPrivilegeLevel&3<<5);
    interruptDescriptorTable[interruptNumber].reserved=0;
}

uint32_t InterruptManger::handleInterrupt(uint8_t InterruptNumber,uint32_t esp){
    printf(" interrupt");

    Port8BitSlow command(0x20);
    command.Write(0x20);

    return esp;
}

