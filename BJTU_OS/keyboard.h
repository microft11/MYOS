#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "types.h"
#include "port.h"
#include "interrupts.h"

class KeyboardEventHandler
{
public:
    KeyboardEventHandler();

    virtual void OnKeyDown(char) {}
    virtual void OnKeyUp(char) {}
};

class KeyboardDriver : public InterruptHandler
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