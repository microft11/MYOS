#ifndef __MOUSE_H__
#define __MOUSE_H__

#include "driver.h"
#include "interrupts.h"
#include "port.h"
#include "types.h"

class MouseEventHandler {
public:
    MouseEventHandler();

    virtual void OnMouseDown(uint8_t button);
    virtual void OnMouseUp(uint8_t button);
    virtual void OnMouseMove(uint16_t xoffset, uint16_t yoffset);
};
/*MouseEventHandler是一个鼠标事件处理器的基类，
它有一个默认构造函数和三个虚函数：OnMouseDown()、OnMouseUp()和OnMouseMove()。
这些函数可以在派生类中进行重写。在基类中，这些函数都是空的，需要在派生类中实现*/

class MouseDriver : public InterruptHandler, public Driver {
public:
    MouseDriver(InterruptManager* manager, MouseEventHandler* handler);
    ~MouseDriver();

    uint32_t HandleInterrupt(uint32_t esp);
    void Activate();

private:
    Port8Bit dataPort;
    Port8Bit commandPort;
    uint8_t buffer[3];
    uint8_t offset;
    uint8_t buttons;

    MouseEventHandler* handler;
};
/*MouseDriver是一个鼠标驱动程序，它继承自InterruptHandler和Driver两个类。
InterruptHandler用于处理中断，Driver用于实现驱动程序的基本行为，但。
在构造函数中，它初始化了数据端口、命令端口和一些成员变量，并将事件处理器对象保存在成员变量中*/
#endif