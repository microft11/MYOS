#ifndef __MYOS__DRIVERS__DRIVER_H__
#define __MYOS__DRIVERS__DRIVER_H__
#include <common/types.h>

namespace myos
{
    namespace drivers
    {
        class Driver
        {
          public:
            Driver();
            ~Driver();

            virtual void Activate();
            virtual void Deactivate();
            virtual int Reset();
        };

        class DriverManager
        {
          public:
            // 构造函数
            DriverManager();
            // Destructor
            ~DriverManager();

            // 激活驱动管理器
            void Activate();
            // 添加驱动程序
            void AddDriver(Driver *driver);

          private:
            // 存储驱动程序的数组
            Driver *drivers[256];
            // 当前驱动程序数量
            int numDrivers;
        };
    } // namespace drivers
} // namespace myos

#endif