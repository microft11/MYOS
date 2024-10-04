#ifndef __MYOS__DRIVERS__KEYBOARD_H__
#define __MYOS__DRIVERS__KEYBOARD_H__

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>

namespace myos
{
    namespace drivers
    {

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

        class KeyboardDriver
            : public myos::hardwarecommunication::InterruptHandler,
              public Driver
        {
          public:
            KeyboardDriver(
                myos::hardwarecommunication::InterruptManager *manager,
                KeyboardEventHandler *handler);
            ~KeyboardDriver();

            virtual void put_buffer(const myos::common::int8_t c);
            virtual myos::common::int8_t *get_buffer(
                myos::common::int8_t *buffer);
            myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
            void Activate();

          private:
            myos::hardwarecommunication::Port8Bit dataPort;
            myos::hardwarecommunication::Port8Bit commandPort;

            struct KB_BUFFER
            {
                myos::common::uint8_t head;
                myos::common::uint8_t tail;
                myos::common::uint8_t count;
                myos::common::uint8_t buf[256];
            };

            static KB_BUFFER kb_buffer;

            KeyboardEventHandler *handler;
        };
    } // namespace drivers
} // namespace myos

#endif // __KEYBOARD_H__