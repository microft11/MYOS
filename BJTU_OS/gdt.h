#ifndef __GDT_H__
#define __GDT_H__
#include "types.h"
class GlobalDescriptorTable // 全局描述符表类
{
  public:
    class SegmentDescriptor // 段描述符类
    {
      private:
        uint16_t limit_lo;      // 段限长低16位
        uint16_t base_lo;       // 段基址低16位
        uint8_t base_hi;        // 段基址高8位
        uint8_t type;           // 段类型
        uint8_t flags_limit_hi; // 标志和段限长高8位
        uint8_t base_vhi;       // 段基址高位

      public:
        SegmentDescriptor(uint32_t base, uint32_t limit,
                          uint8_t type); // 段描述符构造函数
        uint32_t Base();                 // 获取段基址
        uint32_t Limit();                // 获取段限长
    } __attribute__((packed));           // 确保结构体紧凑排列

    SegmentDescriptor nullSegmentSelector;   // 空段选择子
    SegmentDescriptor unusedSegmentSelector; // 未用段选择子
    SegmentDescriptor codeSegmentSelector;   // 代码段选择子
    SegmentDescriptor dataSegmentSelector;   // 数据段选择子

  public:
    GlobalDescriptorTable();  // 全局描述符表构造函数
    ~GlobalDescriptorTable(); // 全局描述符表析构函数

    uint16_t CodeSegmentSelector(); // 获取代码段选择子
    uint16_t DataSegmentSelector(); // 获取数据段选择子
};

#endif