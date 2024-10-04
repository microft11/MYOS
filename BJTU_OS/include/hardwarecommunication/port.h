#ifndef __MYOS__HARDWARECOMMUNICATION__PORT_H__
#define __MYOS__HARDWARECOMMUNICATION__PORT_H__

#include <common/types.h>

namespace myos
{
    namespace hardwarecommunication
    {
        class Port
        {
          protected:
            /* data */
            myos::common::uint16_t portnumber;

          public:
            Port(myos::common::uint16_t portnumber);
            ~Port();
        };

        class Port8Bit : public Port
        {
          public:
            Port8Bit(myos::common::uint16_t portnumber);
            ~Port8Bit();

            virtual void Write(myos::common::uint8_t data);
            virtual myos::common::uint8_t Read();
        };

        class Port8BitSlow : public Port8Bit
        {
          public:
            Port8BitSlow(myos::common::uint16_t portnumber);
            ~Port8BitSlow();

            void Write(myos::common::uint8_t data);
        };

        class Port16Bit : public Port
        {
          public:
            Port16Bit(myos::common::uint16_t portnumber);
            ~Port16Bit();

            void Write(myos::common::uint16_t data);
            myos::common::uint16_t Read();
        };

        class Port32Bit : public Port
        {
          public:
            Port32Bit(myos::common::uint16_t portnumber);
            ~Port32Bit();

            void Write(myos::common::uint32_t data);
            myos::common::uint32_t Read();
        };
    } // namespace hardwarecommunication
} // namespace myos

#endif
