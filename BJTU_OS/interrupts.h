#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include "gdt.h"
#include "multitasking.h"
#include "port.h"
#include "types.h"

class InterruptManager;

class InterruptHandler
{
  protected:
    uint8_t interruptNumber;
    InterruptManager *interruptManager;

  public:
    InterruptHandler(uint8_t interruptNumber,
                     InterruptManager *interruptManager);
    ~InterruptHandler();

    virtual uint32_t HandleInterrupt(uint32_t esp);
};
/*它们互相引用对方的指针。
这是因为 InterruptManager 类需要跟踪并管理一组 InterruptHandler 类的实例，
而 InterruptHandler 类则需要与 InterruptManager
通信，以注册和取消注册自身作为中断处理程序。*/
class InterruptManager
{
    friend class InterruptHandler;

  protected:
    InterruptHandler *handlers[256];
    static InterruptManager *ActivateInterruptManager;
    TaskManager *taskManager;

    struct GateDescriptor
    {
        /* data */
        uint16_t handlerAddressLowBits;   // 中断处理程序入口地址
        uint16_t gdt_codeSegmentSelector; // 段选择子
        uint8_t reserved;                 // 保留位
        uint8_t access;                   // 访问控制位
        uint16_t handlerAddressHighBits;  // 高位字节
    } __attribute__((packed));

    static GateDescriptor interruptDescriptorTable[256];

    struct InterruptsDescriptorTablePointer
    {
        /* data */
        uint16_t size;
        uint32_t base;
    } __attribute__((packed));

    static void SetInterruptDescriptorTableEntry(
        uint8_t InterruptManager,           // 中断号
        uint16_t codeSegmentSelectorOffset, // 段选择符偏移
        void (*handler)(),                  // interrupt处理函数
        uint8_t DescriptorPrivilegeLevel, uint8_t DescriptorType);

    Port8BitSlow picMasterCommand;
    Port8BitSlow picMasterData;
    Port8BitSlow picSlaveCommand;
    Port8BitSlow picSlaveData;

  public:
    InterruptManager(GlobalDescriptorTable *gdt, TaskManager *taskManager);
    ~InterruptManager();

    void Activate();
    void Deactive();

    static uint32_t handleInterrupt(uint8_t Interrupt, uint32_t esp);
    uint32_t DoHandleInterrupt(uint8_t InterruptNumber, uint32_t esp);

    static void IgnoreInterruptRequest();
    static void HandleInterruptRequest0x00();
    static void HandleInterruptRequest0x01();
    static void HandleInterruptRequest0x0C();
    static void HandleInterruptRequest0x60();
};

#endif