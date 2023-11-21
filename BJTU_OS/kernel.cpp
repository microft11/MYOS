#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"
#include "driver.h"
#include "mouse.h"
#include "syscalls.h"

void printf(const int8_t *str)
{
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
            VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | str[i];
            ++x;
            break;
        }
        if(x>=80){
            ++y;
            x=0;
        }
        if(y>=25){
            for(y=0;y<25;++y){
                for(x=0;x<80;x++){
                    VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | str[i];
                }
            }
            x=0;
            y=0;
        }
    }
}

void printfHex(const uint8_t num)
{
    uint8_t c = num;
    static char msg[3] = {'0'};
    const char * hex = "0123456789ABCGEF";
    msg[0] = hex[(c >> 4) & 0xF];
    msg[1] = hex[c & 0xF];
    msg[2] = '\0';
    printf(msg);
}

void sysprintf(const int8_t * str)
{
    __asm__ ("int $0x80": : "a" (4), "b" (str)); //通过软中断，中断进入内核的中断向量表去，然后调用内核里写的print
}

void TaskA()
{
    while(true) {
        sysprintf("A");
    }
}

void TaskB()
{
    while(true) {
        sysprintf("B");
    }
}

class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
public:
    void OnKeyDown(char c)
    {
        char msg[2] = {'\0'};
        msg[0] = c;
        printf(msg);
    }
};

/*代码声明了一个名为VideoMemory的静态成员变量，它是一个指向uint16_t类型的指针，
指向地址0xB8000，该地址是文本模式下屏幕的内存映射起始地址。
通过操作这段内存，可以实现对屏幕上字符的显示和修改。

接下来，MouseToConsole类定义了两个私有成员变量x和y，分别表示鼠标在屏幕上的横坐标和纵坐标。
这些变量用于跟踪鼠标的位置。
在类的构造函数中，x初始化为40，y初始化为12。
然后，代码通过修改VideoMemory[80 * y + x]来改变屏幕上指定位置的字符的颜色。
具体来说，代码通过位操作将字符的颜色的高四位和低四位交换，并保持字符的ASCII码不变。
这样做的效果是，屏幕上的字符颜色发生了变化。

类中还实现了OnMouseMove()、OnMouseDown()和OnMouseUp()函数，用于处理鼠标移动、按下和释放事件。
这些函数首先通过修改VideoMemory[80 * y + x]来改变当前鼠标位置的字符的颜色，然后根据鼠标的移动偏移量更新x和y的值。
在更新x和y之后，代码再次通过修改VideoMemory[80 * y + x]来改变新鼠标位置的字符的颜色。*/
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
        if (x < 0) x = 0;
        if (y >= 80) x = 79;
        y += yoffset;
        if (y < 0) y = 0;
        if (y >= 25) y = 24;

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

uint16_t * MouseToConsole::VideoMemory = (uint16_t *)0xB8000;

typedef void (*constructor)();
extern constructor start_ctors;
extern constructor end_ctors;

extern "C" void callConstructors()
{
        for (constructor *i = &start_ctors; i != &end_ctors; ++i)
    (*i)();
}

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