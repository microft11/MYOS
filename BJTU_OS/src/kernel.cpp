#include <common/types.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>
#include <syscalls.h>

using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

// 这是内核，标注IO库是在操作系统之上实现的，所以用print只能自己写
void printf(const int8_t *str)
{
    // 屏幕缓冲区约定是从0xb8000这里开始的
    static int16_t *VideoMemory = (int16_t *)0xb8000;
    static int8_t x = 0, y = 0;
    for (int32_t i = 0; str[i] != '\0'; ++i)
    {
        switch (str[i])
        {
        case '\n':
            ++y;
            x = 0;
            break;
        default:
            VideoMemory[80 * y + x] =
                (VideoMemory[80 * y + x] &
                 0xFF00) /* 0xFF00为了避免覆盖高位字节 0xb8000*/
                | str[i];
            ++x;
            break;
        }
        if (x >= 80)
        {
            ++y;
            x = 0;
        }
        if (y >= 25)
        {
            for (y = 0; y < 25; ++y)
            {
                for (x = 0; x < 80; x++)
                {
                    VideoMemory[80 * y + x] =
                        (VideoMemory[80 * y + x] & 0xFF00) | str[i];
                }
            }
            x = 0;
            y = 0;
        }
    }
}

void printfHex(const uint8_t num)
{
    uint8_t c = num;
    static char msg[3] = {'0'};
    const char *hex = "0123456789ABCGEF";
    msg[0] = hex[(c >> 4) & 0xF];
    msg[1] = hex[c & 0xF];
    msg[2] = '\0';
    printf(msg);
}

void systime()
{
    __asm__("int $0x80" : : "a"(1));
}

void sysprintf(const int8_t *str)
{
    __asm__(
        "int $0x80"
        :
        : "a"(4),
          "b"(str)); // 通过软中断，中断进入内核的中断向量表去，然后调用内核里写的print
}

int8_t strcmp(const int8_t *src, const int8_t *dest)
{
    while ((*src != '\0') && (*src == *dest))
    {
        src++;
        dest++;
    }
    return *src - *dest;
}
void simpleShell(const int8_t c, KeyboardDriver *pKeyDriver)
{
    if (c == '\n')
    {
        // int8_t cmd[256] = { 0 };
        int8_t *cmd = pKeyDriver->get_buffer(cmd);

        if (strcmp(cmd, "time") == 0)
            systime();
        else if (cmd[0] != '\0')
            sysprintf("ls unknown command\n");

        sysprintf("Ls:>");
    }
}

void TaskA()
{
    while (true)
    {
        sysprintf("A");
    }
}

void TaskB()
{
    while (true)
    {
        sysprintf("B");
    }
}

void time()
{
    Port8Bit out(0x70), in(0x71);
    uint8_t idx = 4;
    out.Write(idx);
    idx = in.Read();
    printfHex(idx);
    printf(":");

    idx = 2;
    out.Write(idx);
    idx = in.Read();
    printfHex(idx);
    printf(":");

    idx = 0;
    out.Write(idx);
    idx = in.Read();
    printfHex(idx);
    printf("\n");
}

class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
  public:
    void OnKeyDown(int8_t c)
    {
        pDriver->put_buffer(c);
        char msg[2] = {'\0'};
        msg[0] = c;
        printf(msg);
    }
};

/*
接下来，MouseToConsole类定义了两个私有成员变量x和y，分别表示鼠标在屏幕上的横坐标和纵坐标。
这些变量用于跟踪鼠标的位置。
在类的构造函数中，x初始化为40，y初始化为12。
然后，代码通过修改VideoMemory[80 * y + x]来改变屏幕上指定位置的字符的颜色。
具体来说，代码通过位操作将字符的颜色的高四位和低四位交换，并保持字符的ASCII码不变。
这样做的效果是，屏幕上的字符颜色发生了变化。

类中还实现了OnMouseMove()、OnMouseDown()和OnMouseUp()函数，用于处理鼠标移动、按下和释放事件。
这些函数首先通过修改VideoMemory[80 * y +
x]来改变当前鼠标位置的字符的颜色，然后根据鼠标的移动偏移量更新x和y的值。
在更新x和y之后，代码再次通过修改VideoMemory[80 * y +
x]来改变新鼠标位置的字符的颜色。*/
class MouseToConsole : public MouseEventHandler
{
    int16_t x, y;
    static uint16_t *VideoMemory;

  public:
    MouseToConsole() : x(40), y(12)
    {
        VideoMemory[80 * y + x] = ((VideoMemory[80 * y + x] & 0xF000) >> 4) |
                                  ((VideoMemory[80 * y + x] & 0xF000) << 4) |
                                  ((VideoMemory[80 * y + x] & 0x00FF));
    }

    void OnMouseMove(int16_t xoffset, int16_t yoffset)
    {
        VideoMemory[80 * y + x] = ((VideoMemory[80 * y + x] & 0xF000) >> 4) |
                                  ((VideoMemory[80 * y + x] & 0xF000) << 4) |
                                  ((VideoMemory[80 * y + x] & 0x00FF));
        x += xoffset;
        if (x < 0)
            x = 0;
        if (y >= 80)
            x = 79;
        y += yoffset;
        if (y < 0)
            y = 0;
        if (y >= 25)
            y = 24;

        VideoMemory[80 * y + x] = ((VideoMemory[80 * y + x] & 0xF000) >> 4) |
                                  ((VideoMemory[80 * y + x] & 0xF000) << 4) |
                                  ((VideoMemory[80 * y + x] & 0x00FF));
    }

    void OnMouseDown(uint8_t button)
    {
        VideoMemory[80 * y + x] = ((VideoMemory[80 * y + x] & 0xF000) >> 4) |
                                  ((VideoMemory[80 * y + x] & 0xF000) << 4) |
                                  ((VideoMemory[80 * y + x] & 0x00FF));
    }
    void OnMouseUp(uint8_t button)
    {
        VideoMemory[80 * y + x] = ((VideoMemory[80 * y + x] & 0xF000) >> 4) |
                                  ((VideoMemory[80 * y + x] & 0xF000) << 4) |
                                  ((VideoMemory[80 * y + x] & 0x00FF));
    }
};

uint16_t *MouseToConsole::VideoMemory = (uint16_t *)0xB8000;

typedef void (*constructor)();
extern constructor start_ctors;
extern constructor end_ctors;

extern "C" void callConstructors()
{
    for (constructor *i = &start_ctors; i != &end_ctors; ++i)
        (*i)();
}

//     *multiboot_structrue *
//     这是指向多引导结构体的指针。当操作系统通过多引导启动时，Bootloader
//     会将这个结构体传递给内核。这个结构体包含了关于启动信息的多种信息，
//     例如内存布局、硬件信息等，内核可以利用这些信息进行初始化和配置。
//     *magicnumber **：这是一个用于验证的值。它的主要作用是告诉内核，Bootloader
//      是否遵循了多引导规范来启动内核。一般情况下，magicnumber
//      的值是一个特定的常量（如
//      0x2BADB002），内核可以通过验证这个值来确保自己是被正确加载的。
extern "C" void kernelMain(void *multiboot_structrue, int32_t magicnumber)
{
    printf("hello OS world!\n");
    printf("hello OS world!\n");

    GlobalDescriptorTable gdt;
    TaskManager taskManager;
    Task task1(&gdt, TaskA);
    Task task2(&gdt, TaskB);
    taskManager.AddTask(&task1);
    taskManager.AddTask(&task2);
    InterruptManager interrupts(&gdt, &taskManager);

    SyscallHandler syscalls(&interrupts);

    DriverManager driverManager;

    PrintfKeyboardEventHandler kbhandler;
    KeyboardDriver keyboard(&interrupts, &kbhandler);

    MouseToConsole mousehandler;
    MouseDriver mouse(&interrupts, &mousehandler);

    driverManager.AddDriver(&keyboard);
    driverManager.AddDriver(&mouse);
    driverManager.Activate();

    interrupts.Activate();

    while (1)
        ;
}