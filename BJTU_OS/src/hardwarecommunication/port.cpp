#include <hardwarecommunication/port.h>

using namespace myos::common;
using namespace myos::hardwarecommunication;

Port::Port(uint16_t portnumber) : portnumber(portnumber)
{
}
Port::~Port()
{
}

// 8bit
Port8Bit::Port8Bit(uint16_t portnumber) : Port(portnumber)
{
}
Port8Bit::~Port8Bit()
{
}
/*这个函数用于向指定的I/O端口写入一个8位宽的数据。
outb 是一个汇编指令，它用于将数据 data 写入指定的I/O端口。
%0 和 %1 是占位符，分别代表指令操作数中的第一个和第二个操作数。
: : "a"(data), "Nd"(portnumber) 是汇编模板的一部分，
它将C++变量 data 的值作为第一个操作数传递给 outb 指令的 %0 占位符，
并将C++变量 portnumber 的值作为第二个操作数传递给 outb 指令的 %1 占位符。
这个函数用于将数据写入指定I/O端口，以进行硬件控制或与外部设备进行通信。*/
void Port8Bit::Write(uint8_t data)
{
    __asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(portnumber));
}
/*这个函数用于从指定的I/O端口读取一个8位宽的数据。
inb 是一个汇编指令，它用于从指定的I/O端口读取数据，并将结果存储在C++变量 result
中。 %0 和 %1 是占位符，分别代表指令操作数中的第一个和第二个操作数。 :
"=a"(result) : "Nd"(portnumber) 是汇编模板的一部分，它将 inb
指令的结果存储在C++变量 result 中， 并将 portnumber 的值作为操作数传递给 inb
指令的 %1 占位符。
这个函数用于从指定I/O端口读取数据，以获取硬件状态或与外部设备进行通信*/
uint8_t Port8Bit::Read()
{
    uint8_t result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(portnumber));
    return result;
}

// 8bitslow
Port8BitSlow::Port8BitSlow(uint16_t portnumber) : Port8Bit(portnumber)
{
}
Port8BitSlow::~Port8BitSlow()
{
}

void Port8BitSlow::Write(uint8_t data)
{
    __asm__ volatile("outb %0, %1 \n jmp 1f \n 1: jmp 1f\n 1:"
                     :
                     : "a"(data), "Nd"(portnumber));
}

// 16bit
Port16Bit::Port16Bit(uint16_t portnumber) : Port(portnumber)
{
}
Port16Bit::~Port16Bit()
{
}
void Port16Bit::Write(uint16_t data)
{
    __asm__ volatile("outw %0, %1" : : "a"(data), "Nd"(portnumber));
}
uint16_t Port16Bit::Read()
{
    uint16_t result;
    __asm__ volatile("inw %1,%0" : "=a"(result) : "Nd"(portnumber));
    return result;
}

// 32bit
Port32Bit::Port32Bit(uint16_t portnumber) : Port(portnumber)
{
}
Port32Bit::~Port32Bit()
{
}
void Port32Bit::Write(uint32_t data)
{
    __asm__ volatile("outl %0, %1" : : "a"(data), "Nd"(portnumber));
}
uint32_t Port32Bit::Read()
{
    uint32_t result;
    __asm__ volatile("inl %1,%0" : "=a"(result) : "Nd"(portnumber));
    return result;
}
