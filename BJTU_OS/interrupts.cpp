#include "interrupts.h"
void printf(const int8_t*);
InterruptManger::GateDescriptor InterruptManger::interruptDescriptorTable[256];

InterruptManger::InterruptManger(GlobalDescriptorTable *gdt)
    : picMasterCommand(0x20), picMasterData(0x21), picSlaveCommand(0xA0), picSlaveData(0xA1)
{
    uint16_t CodeSegment = gdt->CodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    for (uint16_t i = 0; i < 256; i++)
    {
        SetInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
    }

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

    InterruptsDescriptorTablePointer idt;
    idt.size = 256 *sizeof(GateDescriptor)-1;
    idt.base = (uint32_t)interruptDescriptorTable;

    __asm__ volatile("lidt %0": :"m" (idt));
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

