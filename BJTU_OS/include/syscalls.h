#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

// #include "multitasking.h"
#include <common/types.h>
#include <hardwarecommunication/interrupts.h>

using namespace myos::hardwarecommunication;

class SyscallHandler : public InterruptHandler
{
  public:
    SyscallHandler(InterruptManager *interruptManager);
    ~SyscallHandler();

    myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
};

#endif