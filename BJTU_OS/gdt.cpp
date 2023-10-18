#include "gdt.h"

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t type)
{
    uint8_t *target = (uint8_t *)this;

    /*根据x86架构的要求，限制字段有不同的编码方式。
    如果限制小于等于65536（64KB），则使用一种编码方式，否则使用另一种编码方式。*/
    if (limit <= 65536)
    {
        target[6] = 0x40;
    }
    else
    {
        if ((limit & 0xFFF) != 0xFFF)
        {
            limit = (limit >> 12) - 1;
        }
        else
        {
            limit = limit >> 12;
        }

        target[6] = 0xC0;
    }

    target[0] = limit & 0xFF;
    target[1] = (limit >> 8) & 0xFF;
    target[6] = target[6] | ((limit >> 16) & 0xF);

    target[2] = base & 0xFF;
    target[3] = (base >> 8) & 0xFF;
    target[4] = (base >> 16) & 0xFF;
    target[7] = (base >> 24) & 0xFF;

    target[5] = type;
};

uint32_t GlobalDescriptorTable::SegmentDescriptor::Base()
{
    uint8_t *target = (uint8_t *)this;
    uint32_t result = target[7];
    result = (result << 8) + target[4];
    result = (result << 8) + target[3];
    result = (result << 8) + target[2];
    return result;
    /*包含了从段描述符中提取的完整基地址，这个基地址是一个32位无符号整数。
    总之，这个函数用于从段描述符中提取并重构段的起始地址，以便后续在代码中使用。
    这对于加载和使用段非常有用，例如在x86体系结构中用于确定代码段或数据段的物理地址*/
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Limit()
{
    uint8_t *target = (uint8_t *)this;
    uint32_t result = target[6] & 0xF;
    result = (result << 8) + target[1];
    result = (result << 8) + target[0];

    if((target[6]&0xC0)==0xC0){
        result = (result<<12)|0xFFF;
    }
    return result;
}

// GlobalDescriptorTable

GlobalDescriptorTable::GlobalDescriptorTable()
    : nullSegmentSelector(0, 0, 0),
      unusedSegmentSelector(0, 0, 0),
      codeSegmentSelector(0, 64 * 1024*1024, 0x9A),
      dataSegmentSelector(0, 64 * 1024*1024, 0x92)
{
    uint32_t i[2];
    i[1] = (uint32_t)this;
    i[0] = sizeof(GlobalDescriptorTable) << 16;
    /*在x86体系结构中，GDTR（全局描述符表寄存器）的格式是一个48位的寄存器，
    其中低16位存储 GDT 表的大小
    而高32位存储 GDT 表的线性地址。
    左移16位将 sizeof(GlobalDescriptorTable) 的值从字节转换为字（1字节 = 8位）*/

    __asm__ volatile("lgdt (%0)" : : "p"((uint8_t *)i + 2));
    /*•	构建一个描述GDT的32位整数数组 i，其中包含 GDT 表的地址和大小。
•	使用汇编内联指令 lgdt，将 GDT 表的地址和大小加载到全局描述符寄存器（GDTR）中。
*/
}

GlobalDescriptorTable::~GlobalDescriptorTable()
{
}

uint16_t GlobalDescriptorTable::CodeSegmentSelector()
{
    // 返回代码段和数据段的选择符
    return (uint8_t *)&codeSegmentSelector - (uint8_t *)this;
}

uint16_t GlobalDescriptorTable::DataSegmentSelector()
{
    // 返回代码段和数据段的选择符
    return (uint8_t *)&dataSegmentSelector - (uint8_t *)this;
}
/*这个偏移量通常用于在汇编代码中构建段选择器，以便将其加载到段寄存器中
这是x86体系结构中设置段的一种典型方式。
这个偏移量可以被用来设置段寄存器，如CS（代码段寄存器）或DS（数据段寄存器），
以指向相应的段描述符，从而确定当前运行代码的权限和所处的段。
在操作系统内核或底层系统编程中，这样的计算通常是必需的，以正确配置内存访问权限。*/