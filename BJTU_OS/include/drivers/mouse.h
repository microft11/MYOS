#ifndef __MYOS__DRIVERS__MOUSE_H__
#define __MYOS__DRIVERS__MOUSE_H__

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>

namespace myos
{
    namespace drivers
    {
        class MouseEventHandler
        {
          public:
            MouseEventHandler();

            virtual void OnMouseDown(myos::common::uint8_t button);
            virtual void OnMouseUp(myos::common::uint8_t button);
            virtual void OnMouseMove(myos::common::uint16_t xoffset,
                                     myos::common::uint16_t yoffset);
        };
        /*MouseEventHandler是一个鼠标事件处理器的基类，
        它有一个默认构造函数和三个虚函数：OnMouseDown()、OnMouseUp()和OnMouseMove()。
        这些函数可以在派生类中进行重写。在基类中，这些函数都是空的，需要在派生类中实现*/

        class MouseDriver
            : public myos::hardwarecommunication::InterruptHandler,
              public Driver
        {
          public:
            MouseDriver(myos::hardwarecommunication::InterruptManager *manager,
                        MouseEventHandler *handler);
            ~MouseDriver();

            myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
            void Activate();

          private:
            myos::hardwarecommunication::Port8Bit dataPort;
            myos::hardwarecommunication::Port8Bit commandPort;
            myos::common::uint8_t buffer[3];
            myos::common::uint8_t offset;
            myos::common::uint8_t buttons;

            MouseEventHandler *handler;
        };
        /*MouseDriver是一个鼠标驱动程序，它继承自InterruptHandler和Driver两个类。
        InterruptHandler用于处理中断，Driver用于实现驱动程序的基本行为，但。
        在构造函数中，它初始化了数据端口、命令端口和一些成员变量，并将事件处理器对象保存在成员变量中*/
    } // namespace drivers
} // namespace myos

#endif