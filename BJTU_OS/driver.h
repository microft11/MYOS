#ifndef __DRIVER_H__
#define __DRIVER_H__
#include "types.h"

class Driver
{
  public:
    Driver();
    ~Driver();

    virtual void Activate();
    virtual void Deactivate();
    virtual int Reset();
};
/*Driver类是一个基类，它有一个默认构造函数和析构函数。
它还定义了几个虚函数：Activate()、Deactivate()和Reset()。
这些函数是虚函数，意味着它们可以在派生类中被重写。
Activate()函数用于激活驱动程序，Deactivate()函数用于停用驱动程序，
Reset()函数用于重置驱动程序。
在基类中，这些函数都是空的，需要在派生类中进行实现。*/

class DriverManager
{
  public:
    DriverManager();
    ~DriverManager();

    void Activate();
    void AddDriver(Driver *driver);

  private:
    Driver *drivers[256];
    int numDrivers;
};
/*DriverManager类是一个驱动程序管理器，它有一个默认构造函数和析构函数。
它包含了一个指向Driver对象的指针数组drivers，以及一个表示驱动程序数量的整数numDrivers。
构造函数中初始化了drivers数组为nullptr，并将numDrivers初始化为0。

DriverManager类还定义了两个函数：Activate()和AddDriver()。
Activate()函数用于激活所有的驱动程序，它通过遍历drivers数组并调用每个驱动程序的Activate()函数实现。
AddDriver()函数用于向驱动程序管理器中添加驱动程序。
它将指向驱动程序对象的指针添加到drivers数组中，并增加numDrivers的值。*/

#endif