#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "driver.h"
#include "interrupts.h"
#include "port.h"
#include "types.h"

class KeyboardDriver;
class KeyboardEventHandler
{
  public:
    KeyboardEventHandler();

    void SetDriver(KeyboardDriver *pDriver);
    virtual void OnKeyDown(char)
    {
    }
    virtual void OnKeyUp(char)
    {
    }

  protected:
    KeyboardDriver *pDriver;
};
/*KeyboardEventHandler是一个事件处理器的基类，它有一个默认构造函数和两个虚函数：
OnKeyDown()和OnKeyUp()。
这些函数可以在派生类中进行重写。
在基类中，这些函数都是空的，需要在派生类中实现*/

class KeyboardDriver : public InterruptHandler, public Driver
{
  public:
    KeyboardDriver(InterruptManager *manager, KeyboardEventHandler *handler);
    ~KeyboardDriver();

    virtual void put_buffer(const int8_t c);
    virtual int8_t *get_buffer(int8_t *buffer);
    uint32_t HandleInterrupt(uint32_t esp);
    void Activate();

  private:
    Port8Bit dataPort;
    Port8Bit commandPort;

    struct KB_BUFFER
    {
        uint8_t head;
        uint8_t tail;
        uint8_t count;
        uint8_t buf[256];
    };

    static KB_BUFFER kb_buffer;

    KeyboardEventHandler *handler;
};

#endif // __KEYBOARD_H__