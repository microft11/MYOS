#include <hardwarecommunication/interrupts.h>

using namespace myos::common;
using namespace myos::hardwarecommunication;

void printf(const int8_t *);
void printfHex(const uint8_t);

// 中断处理器
InterruptHandler::InterruptHandler(uint8_t interruptNumber,
                                   InterruptManager *interruptManager)
    : interruptNumber(interruptNumber), interruptManager(interruptManager)
{ // 这里指回了interruptmanager，指回的这个是惟一的，用来告诉派生了什么新的方法
    interruptManager->handlers[interruptNumber] = this;
}

InterruptHandler::~InterruptHandler()
{
    if (interruptManager->handlers[interruptNumber] == this)
        interruptManager->handlers[interruptNumber] = 0;
}

/*这个函数用于处理中断。默认情况下，它只返回传入的栈指针
esp，没有特定的中断处理操作。
需要注意，这个函数是虚函数，可以在派生类中进行覆盖以执行特定的中断处理操作。*/
uint32_t InterruptHandler::HandleInterrupt(uint32_t esp)
{
    return esp;
}

// 存储中断描述符表（IDT）。IDT
// 包含256个中断描述符，每个描述符用于描述一个中断处理例程
InterruptManager::GateDescriptor
    InterruptManager::interruptDescriptorTable[256];

// 用于跟踪当前激活的中断管理器
InterruptManager *InterruptManager::ActivateInterruptManager = 0;

InterruptManager::InterruptManager(GlobalDescriptorTable *gdt,
                                   TaskManager *taskManager)
    : picMasterCommand(0x20), picMasterData(0x21), picSlaveCommand(0xA0),
      picSlaveData(0xA1), taskManager(taskManager)
{
    uint16_t CodeSegment = gdt->CodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;
    // 定义了中断门描述符的类型

    for (uint16_t i = 0; i < 256; i++)
    {
        handlers[i] = 0;
        SetInterruptDescriptorTableEntry(
            i, CodeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
    }
    // 通过 for 循环，遍历256个中断号（0-255），对每个中断号都调用
    // SetInterruptDescriptorTableEntry 函数来设置 IDT表的中断门描述符。
    // 每个中断门描述符的中断处理函数都被设置为
    // IgnoreInterruptRequest，这个函数用于忽略中断请求，相当于占位符。

    SetInterruptDescriptorTableEntry(
        0x20, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(
        0x21, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(
        0x2C, CodeSegment, &HandleInterruptRequest0x0C, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(
        0x80, CodeSegment, &HandleInterruptRequest0x60, 0, IDT_INTERRUPT_GATE);

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
    配置主PIC和从PIC的中断向量起始地址。主PIC的起始地址为
    0x20，从PIC的起始地址为 0x28。 通过 picMasterData.Write(0x04) 和
    picSlaveData.Write(0x02) 来告诉PIC它们的中断控制关系（主从关系）。
    最后，通过 picMasterData.Write(0x00) 和 picSlaveData.Write(0x00)
    来设置中断屏蔽码，将所有中断都关闭。*/

    InterruptsDescriptorTablePointer
        idt; // 存储 IDT 表的指针和大小信息的数据结构
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    idt.base = (uint32_t)interruptDescriptorTable;

    __asm__ volatile("lidt %0" : : "m"(idt));
    //     "lidt %0" 是 lidt 汇编指令的操作数，%0
    //     冒号后面的内容是汇编指令的输入和输出参数。在这种情况下，没有输出参数，只有一个输入参数，即
    //    idt 结构体的地址。 "m" 表示将 idt 结构体作为内存操作数传递给 lidt 指令
}

InterruptManager::~InterruptManager()
{
}

void InterruptManager::Activate()
{
    if (ActivateInterruptManager != nullptr)
        ActivateInterruptManager->Deactive();
    ActivateInterruptManager = this;

    __asm__ volatile("sti");
}

void InterruptManager::Deactive()
{
    if (ActivateInterruptManager != this)
    {
        ActivateInterruptManager = 0;
        __asm__ volatile("cli");
    }
}

/*这个函数用于设置 IDT 表中的中断描述符。
它接受中断号、段选择符、中断处理函数、描述符特权级别和描述符类型等参数
然后填充 IDT 表的相应条目*/
void InterruptManager::SetInterruptDescriptorTableEntry(
    uint8_t interruptNumber,            // 中断号
    uint16_t codeSegmentSelectorOffset, // 段选择符偏移
    void (*handler)(),                  // interrupt处理函数
    uint8_t DescriptorPrivilegeLevel, uint8_t DescriptorType)
{
    const uint8_t IDT_DESC_PRESENT = 0x80;
    interruptDescriptorTable[interruptNumber].handlerAddressLowBits =
        ((uint32_t)handler) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].handlerAddressHighBits =
        ((uint32_t)handler >> 16) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector =
        codeSegmentSelectorOffset;
    interruptDescriptorTable[interruptNumber].access =
        IDT_DESC_PRESENT | DescriptorType | (DescriptorPrivilegeLevel & 3 << 5);
    interruptDescriptorTable[interruptNumber].reserved = 0;
}

/*这个函数是一个处理中断的入口函数。
它会根据当前激活的中断管理器（ActivateInterruptManager）来调用相应的中断处理函数。
如果没有激活的中断管理器，它仅返回传入的栈指针 esp*/
uint32_t InterruptManager::handleInterrupt(uint8_t InterruptNumber,
                                           uint32_t esp)
{
    // printf(" interrupt");

    if (ActivateInterruptManager != 0)
        return ActivateInterruptManager->DoHandleInterrupt(InterruptNumber,
                                                           esp);

    // Port8BitSlow command(0x20);
    // command.Write(0x20);

    return esp;
}

/*这个函数用于实际处理中断。
它会首先检查是否有注册的中断处理器，如果有，就调用该处理器的 HandleInterrupt
函数。
如果没有注册的处理器，并且中断号不是0x20（时钟中断），则输出一条错误信息。
最后，它会更新 PIC 控制器，通知 PIC 中断已经被处理。*/
uint32_t InterruptManager::DoHandleInterrupt(uint8_t InterruptNumber,
                                             uint32_t esp)
{
    if (handlers[InterruptNumber] != nullptr)
        esp = handlers[InterruptNumber]->HandleInterrupt(esp);
    else if (InterruptNumber != 0x20)
    {
        char msg[] = " unhandled interrupt 0x00";
        printfHex(InterruptNumber);
    }

    if (InterruptNumber == 0x20)
        esp = (uint32_t)taskManager->Schedule(
            (CPUState *)esp); // 在时间中断处添加事件切换

    if (0x20 <= InterruptNumber && InterruptNumber < 0x30)
    {
        picMasterCommand.Write(0x20);
        if (0x28 <= InterruptNumber)
            picSlaveCommand.Write(0x20);
    }
    return esp;
}
