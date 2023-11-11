#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "types.h"
#include "port.h"
#include "interrupts.h"
#include "driver.h"

class KeyboardEventHandler
{
public:
    KeyboardEventHandler();

    virtual void OnKeyDown(char) {}
    virtual void OnKeyUp(char) {}
};
/*KeyboardEventHandler是一个事件处理器的基类，它有一个默认构造函数和两个虚函数：OnKeyDown()和OnKeyUp()。
这些函数可以在派生类中进行重写。在基类中，这些函数都是空的，需要在派生类中实现*/

class KeyboardDriver : public InterruptHandler, public Driver
{
public:
    KeyboardDriver(InterruptManager * manager, KeyboardEventHandler * handler);
    ~KeyboardDriver();

    uint32_t HandleInterrupt(uint32_t esp);
    void Activate();

private:
    Port8Bit dataPort;
    Port8Bit commandPort;

    KeyboardEventHandler * handler;
};

#endif // __KEYBOARD_H__