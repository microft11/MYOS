#ifndef __MYOS__HARDWARECOMMUNICATION__PCI_H
#define __MYOS__HARDWARECOMMUNICATION__PCI_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>

namespace myos
{
    namespace hardwarecommunication
    {

        enum BaseAddressRegisterType
        {
            MemoryMapping = 0,
            InputOutput = 1
        };

        class BaseAddressRegister
        {
          public:
            bool prefetchable;
            myos::common::uint8_t *address;
            myos::common::uint32_t size;
            BaseAddressRegisterType type;
        };

        // 表示外设组件互联（PCI）设备描述符类
        class PeripheralComponentInterconnectDeviceDescriptor
        {
          public:
            myos::common::uint32_t portBase;  // 端口基址
            myos::common::uint32_t interrupt; // 中断号

            myos::common::uint16_t bus;      // 总线号
            myos::common::uint16_t device;   // 设备号
            myos::common::uint16_t function; // 功能号

            myos::common::uint16_t vendor_id; // 供应商ID
            myos::common::uint16_t device_id; // 设备ID

            myos::common::uint8_t class_id;     // 类别ID
            myos::common::uint8_t subclass_id;  // 子类别ID
            myos::common::uint8_t interface_id; // 接口ID

            myos::common::uint8_t revision; // 版本号

            PeripheralComponentInterconnectDeviceDescriptor();
            ~PeripheralComponentInterconnectDeviceDescriptor();
        };
        class PeripheralComponentInterconnectController
        {
          private:
            Port32Bit dataPort;
            Port32Bit commandPort;

          public:
            PeripheralComponentInterconnectController();
            ~PeripheralComponentInterconnectController();

            myos::common::uint32_t Read(myos::common::uint16_t bus,
                                        myos::common::uint16_t device,
                                        myos::common::uint16_t function,
                                        myos::common::uint32_t registeroffset);
            void Write(myos::common::uint16_t bus,
                       myos::common::uint16_t device,
                       myos::common::uint16_t function,
                       myos::common::uint32_t registeroffset,
                       myos::common::uint32_t value);
            bool DeviceHasFunctions(myos::common::uint16_t bus,
                                    myos::common::uint16_t device);

            void SelectDrivers(
                myos::drivers::DriverManager *driverManager,
                myos::hardwarecommunication::InterruptManager *interrupts);

            myos::drivers::Driver *GetDriver(
                PeripheralComponentInterconnectDeviceDescriptor dev,
                myos::hardwarecommunication::InterruptManager *interrupts);

            PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(
                myos::common::uint16_t bus, myos::common::uint16_t device,
                myos::common::uint16_t function);

            BaseAddressRegister GetBaseAddressRegister(
                myos::common::uint16_t bus, myos::common::uint16_t device,
                myos::common::uint16_t function, myos::common::uint16_t bar);
        };
    } // namespace hardwarecommunication
} // namespace myos

#endif