#ifndef __MULTITASKING_H__
#define __MULTITASKING_H__

#include "types.h"
#include "gdt.h"

/*表示在上下文切换期间CPU的状态。
它包括通用寄存器（eax、ebx、ecx、edx、esi、edi、ebp）、
错误码、指令指针（eip）、代码段（cs）、标志寄存器（eflags）、栈指针（esp）和栈段（ss）。
__attribute__ ((packed)) 用于确保结构体成员之间没有填充字节*/
struct CPUState 
{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;

    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;

    // uint32_t gs;
    // uint32_t fs;
    // uint32_t es;
    // uint32_t ds;

    uint32_t error;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} __attribute__ ((packed));


/*Task 类表示多任务环境中的一个任务。
它有一个私有的大小为4096字节的栈数组和一个指向 CPUState 结构的私有指针。
它与 TaskManager 类有友元关系，表示 TaskManager 可以访问 Task 的私有成员。
它有一个构造函数，接受一个指向 GlobalDescriptorTable 和一个函数指针 entrypoint 的参数，还有一个析构函数。*/
class Task
{
    friend class TaskManager;
public:
    Task(GlobalDescriptorTable* gdt, void entrypoint());
    ~Task();    
private:
    uint8_t stack[4096];
    CPUState * cpustate;
};
/*为什么需要传入 GDT 的一些原因：
设置代码段和数据段： GDT 包含了描述任务代码段和数据段属性的描述符。
通过传递 GDT，任务能够知道自己的代码和数据所在的段，以便在任务切换时正确地加载这些段。

获取段选择子： 任务需要知道它的代码段和数据段在 GDT 中的选择子。
构造函数中通过 gdt->CodeSegmentSelector() 获取了代码段的选择子，
并将其存储在任务的 CPU 状态结构（cpustate）中的 cs 字段中。

设置任务入口点： 任务的入口点是任务开始执行的地址。
构造函数中通过传递的 entrypoint 函数指针，将任务的 CPU 状态结构中的 eip 字段设置为入口点的地址。

确保正确的权限和访问： GDT 中的描述符定义了各个段的权限和访问规则。
通过使用正确的 GDT，任务可以确保它有权访问其代码和数据，并且这些段的属性符合操作系统的要求。*/

/*TaskManager 类管理一组任务。
它有一个构造函数、一个析构函数以及添加任务 (AddTask) 和调度任务 (Schedule) 的方法。
它维护了一个 Task 指针数组（tasks），最多包含256个任务。
它追踪任务数量（numTasks）和当前正在执行的任务的索引（currentTask）。*/
class TaskManager
{
public:
    TaskManager();
    ~TaskManager();
    bool AddTask(Task * task);
    CPUState * Schedule(CPUState * cpustate);
private:
    Task * tasks[256];
    uint8_t numTasks;
    uint8_t currentTask;
};

#endif