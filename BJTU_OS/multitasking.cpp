#include "multitasking.h"

/*在构造函数中，为任务分配栈空间，然后初始化任务的 CPU 状态结构 cpustate。
entrypoint 参数是一个函数指针，表示任务的入口点（即任务将从这个函数开始执行）。
cpustate->eip 被设置为 entrypoint，cpustate->cs
被设置为代码段选择子，cpustate->eflags 设置为默认值。
这个构造函数的目的是为新任务初始化执行环境*/
Task::Task(GlobalDescriptorTable *gdt, void entrypoint())
{
    cpustate = (CPUState *)(stack + 4096 - sizeof(CPUState));

    cpustate->eax = 0;
    cpustate->ebx = 0;
    cpustate->ecx = 0;
    cpustate->edx = 0;

    cpustate->esi = 0;
    cpustate->edi = 0;
    cpustate->ebp = 0;

    // cpustate->gs = 0;
    // cpustate->fs = 0;
    // cpustate->es = 0;
    // cpustate->ds = 0;

    cpustate->error = 0;

    cpustate->eip = (uint32_t)entrypoint; // 任务的入口地址
    cpustate->cs = gdt->CodeSegmentSelector();
    cpustate->eflags = 0x202;
    // cpustate->esp = 0;
    // cpustate->ss = 0;
}

Task::~Task()
{
}

TaskManager::TaskManager() : numTasks(0), currentTask(-1)
{
}

TaskManager::~TaskManager()
{
}
bool TaskManager::AddTask(Task *task)
{
    if (numTasks >= 255)
        return false;
    else
    {
        tasks[numTasks++] = task;
        return true;
    }
}
/*Schedule 方法用于任务调度，即切换当前执行的任务。
如果没有任务，则返回传入的 cpustate。
如果有当前任务，将其 CPU 状态更新为传入的 cpustate。
递增 currentTask，并确保不超过任务数量。然后返回新的任务的 CPU 状态*/
CPUState *TaskManager::Schedule(CPUState *cpustate)
{
    if (numTasks <= 0)
        return cpustate;
    if (currentTask >= 0)
        tasks[currentTask]->cpustate = cpustate;
    if (++currentTask >= numTasks)
        currentTask = 0;

    return tasks[currentTask]->cpustate;
}
