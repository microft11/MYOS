#ifndef __MOUSE_H__
#define __MOUSE_H__

#include "types.h"
#include "port.h"
#include "interrupts.h"
#include "driver.h"

class MouseEventHandler
{
public:
    MouseEventHandler();

    virtual void OnMouseDown(uint8_t button);
    virtual void OnMouseUp(uint8_t button);
    virtual void OnMouseMove(uint16_t xoffset, uint16_t yoffset);
};

class MouseDriver : public InterruptHandler, public Driver
{
public:
    MouseDriver(InterruptManager * manager, MouseEventHandler * handler);
    ~MouseDriver();

    uint32_t HandleInterrupt(uint32_t esp);
    void Activate();

private:
    Port8Bit dataPort;
    Port8Bit commandPort;
    uint8_t buffer[3];
    uint8_t offset;
    uint8_t buttons;

    MouseEventHandler * handler;
};

#endif