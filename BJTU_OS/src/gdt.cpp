#include <gdt.h>
using namespace myos::common;

/**
 * @brief 段描述符构造函数，用于初始化一个符合x86架构标准的段描述符。
 *
 * 该构造函数通过将输入的段基地址（base）、段限长（limit）和段类型（type）
 * 编码为符合x86架构要求的段描述符格式，用于内存分段的管理。在x86架构中，段描述符
 * 包含段基地址、段限长、段类型、粒度标志等信息，定义了程序在保护模式下可以访问的
 * 内存段的属性。
 *
 * @param base 段的基地址（32位），定义段的起始地址。
 * @param limit 段的限长（20位），定义段的最大偏移量，即段的大小。
 * @param type 段的类型和访问权限，定义段的行为，比如代码段、数据段等。
 */
GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(uint32_t base,
                                                            uint32_t limit,
                                                            uint8_t type)
{
    // 将当前对象的地址强制转换为字节指针，以便按字节操作段描述符的每一部分。
    uint8_t *target = (uint8_t *)this;

    /**
     * 判断段限长（limit）的大小，并根据其大小决定使用字节粒度（1字节为单位）或
     * 页粒度（4KB为单位）来表示段限长。
     */
    if (limit <= 65536)
    {
        // 如果段限长小于等于 64KB，使用字节粒度（0x40 表示字节粒度）。
        target[6] = 0x40;
    }
    else
    {
        // 如果段限长大于 64KB，使用页粒度（4KB 为单位）。

        // 判断 limit 的低 12 位是否全为 1，决定是否需要对 limit 进行调整。
        if ((limit & 0xFFF) != 0xFFF)
        {
            // 如果 limit 不是页对齐（4KB的倍数），则向右移 12 位，并减去 1。
            limit = (limit >> 12) - 1;
        }
        else
        {
            // 如果 limit 已经是页对齐的值，直接向右移 12 位。
            limit = limit >> 12;
        }

        // 使用页粒度（0xC0 表示页粒度）。
        target[6] = 0xC0;
    }

    // 将 limit 的低 16 位存储到 target[0] 和 target[1] 中。
    target[0] = limit & 0xFF;        // 段限长的低 8 位
    target[1] = (limit >> 8) & 0xFF; // 段限长的高 8 位
    target[6] =
        target[6] | ((limit >> 16) & 0xF); // 段限长的高 4 位合并到 target[6] 中

    // 将 base 的低 32 位分段存储到 target[2] 到 target[7] 中。
    target[2] = base & 0xFF;         // 段基址的低 8 位
    target[3] = (base >> 8) & 0xFF;  // 段基址的第 8 到 15 位
    target[4] = (base >> 16) & 0xFF; // 段基址的第 16 到 23 位
    target[7] = (base >> 24) & 0xFF; // 段基址的第 24 到 31 位

    // 设置段的类型字段（包括访问权限等），存储在 target[5] 中。
    target[5] = type;
};

// 该函数用于从段描述符中提取段的基地址。返回值是一个32位无符号整数，表示段的起始地址。
uint32_t GlobalDescriptorTable::SegmentDescriptor::Base()
{
    uint8_t *target = (uint8_t *)this;
    uint32_t result = target[7];
    result = (result << 8) + target[4];
    result = (result << 8) + target[3];
    result = (result << 8) + target[2];
    return result;
}

/**
 * @brief 获取段描述符的限制值（Limit）。
 *
 * 该函数根据段描述符中的字节计算段的有效限制值。
 * @return 段的限制值。
 */
uint32_t GlobalDescriptorTable::SegmentDescriptor::Limit()
{
    uint8_t *target = (uint8_t *)this;
    uint32_t result = target[6] & 0xF;
    result = (result << 8) + target[1];
    result = (result << 8) + target[0];

    if ((target[6] & 0xC0) == 0xC0)
    {
        result = (result << 12) | 0xFFF;
    }
    return result;
}

// GlobalDescriptorTable

GlobalDescriptorTable::GlobalDescriptorTable()
    : nullSegmentSelector(0, 0, 0), unusedSegmentSelector(0, 0, 0),
      codeSegmentSelector(0, 64 * 1024 * 1024, 0x9A),
      dataSegmentSelector(0, 64 * 1024 * 1024, 0x92)
{
    // 用于存储GDT的大小和地址信息。
    // 这个数组将用作lgdt指令的参数，告诉CPU
    // GDT的位置和大小
    uint32_t i[2];

    // 将当前GDT对象的地址this存储到数组i的第二个元素中，这表示GDT的基址
    i[1] = (uint32_t)this;
    // 左移16位后，GDT的大小被放在i[0]的高16位中。
    // 低16位是为GDT的段限长（size）保留的
    i[0] = sizeof(GlobalDescriptorTable) << 16;

    // lgdt是x86汇编中的一条指令，用于加载全局描述符表寄存器（GDTR）。
    // 这条指令将描述符表的位置和大小加载到CPU中，以便在系统运行时使用新的GDT。
    __asm__ volatile(
        "lgdt (%0)"
        :
        : "p"((uint8_t *)i + 2)); // 跳过前两个字节直接指向GDT的实际内容
}

GlobalDescriptorTable::~GlobalDescriptorTable()
{
}

// 将codeSegmentSelector的地址减去this的地址
// 得到的是codeSegmentSelector相对于当前对象起始地址的偏移量。
// 这个偏移量实际上就是段选择符的值
uint16_t GlobalDescriptorTable::CodeSegmentSelector()
{
    // 返回代码段的选择符
    return (uint8_t *)&codeSegmentSelector - (uint8_t *)this;
}

uint16_t GlobalDescriptorTable::DataSegmentSelector()
{
    // 返回数据段的选择符
    return (uint8_t *)&dataSegmentSelector - (uint8_t *)this;
}