#include "keyboard.h"

void printf(const char *);
void printfHex(const uint8_t); // 用于以十六进制格式输出一个字节的数据
void simpleShell(const int8_t c, KeyboardDriver *pKeyDriver);

KeyboardEventHandler::KeyboardEventHandler()
{
}

void KeyboardEventHandler::SetDriver(KeyboardDriver *pDriver)
{
    this->pDriver = pDriver;
}

KeyboardDriver::KB_BUFFER KeyboardDriver::kb_buffer = {0};

/*一个是 InterruptManager 类的指针 manager，另一个是 KeyboardEventHandler
类的指针 handler。
在构造函数的实现中，它初始化了各种成员变量，包括中断处理器、数据端口、命令端口和事件处理器
0x60是键盘的数据端口，0x64是键盘的控制端口*/
KeyboardDriver::KeyboardDriver(InterruptManager *manager,
                               KeyboardEventHandler *handler)
    : InterruptHandler(0x21, manager), dataPort(0x60), commandPort(0x64),
      handler(handler)
{
    this->handler->SetDriver(this);
}

KeyboardDriver::~KeyboardDriver()
{
}

void KeyboardDriver::put_buffer(const int8_t c)
{
    if (kb_buffer.count < 255)
    {
        kb_buffer.count++;
        kb_buffer.buf[kb_buffer.tail++] = c;
        if (kb_buffer.tail == 255)
            kb_buffer.tail = 0;
    }
}
int8_t *KeyboardDriver::get_buffer(int8_t *buffer)
{
    int16_t i = 0;
    int8_t c = kb_buffer.buf[kb_buffer.head];
    buffer[i] = c;
    while (kb_buffer.count > 0 && c != '\n')
    {
        kb_buffer.head++;
        kb_buffer.count--;
        c = kb_buffer.buf[kb_buffer.head];
        buffer[++i] = c;
        if (kb_buffer.head == 255)
            kb_buffer.head = 0;
    }
    kb_buffer.head++;
    kb_buffer.count--;
    if (kb_buffer.head == 255)
        kb_buffer.head = 0;

    buffer[i] = '\0';
    return buffer;
}

/*这个函数是用于处理键盘中断的方法。当键盘中断发生时，操作系统将调用此函数。
函数返回一个 uint32_t 类型的值，并且接受一个 esp 参数（栈指针）。
在函数内部，它读取键盘输入，并根据输入的键码执行不同的操作，例如发送字符到
handler，或者输出键码。

函数中，有一系列 case 语句，每个 case 对应一个键盘扫描码。
根据键盘输入的不同，它们执行不同的操作，例如将字符发送到事件处理器，输出字符，设置Shift状态等。
键盘输入的处理逻辑似乎是基于扫描码来映射字符*/
uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t key = dataPort.Read();
    if (handler == 0)
        return esp;
    static bool shift = false;

    char msg[2] = {'\0'};
    switch (key)
    {
    case 0x45:
        break;
    case 0x1E:
        if (shift)
            handler->OnKeyDown('A');
        else
            handler->OnKeyDown('a');
        break;
    case 0x30:
        if (shift)
            handler->OnKeyDown('B');
        else
            handler->OnKeyDown('b');
        break;
    case 0x2E:
        if (shift)
            handler->OnKeyDown('C');
        else
            handler->OnKeyDown('c');
        break;
    case 0x20:
        if (shift)
            handler->OnKeyDown('D');
        else
            handler->OnKeyDown('d');
        break;
    case 0x12:
        if (shift)
            handler->OnKeyDown('E');
        else
            handler->OnKeyDown('e');
        break;
    case 0x21:
        if (shift)
            handler->OnKeyDown('F');
        else
            handler->OnKeyDown('f');
        break;
    case 0x22:
        if (shift)
            handler->OnKeyDown('G');
        else
            handler->OnKeyDown('g');
        break;
    case 0x23:
        if (shift)
            handler->OnKeyDown('H');
        else
            handler->OnKeyDown('h');
        break;
    case 0x17:
        if (shift)
            handler->OnKeyDown('I');
        else
            handler->OnKeyDown('i');
        break;
    case 0x24:
        if (shift)
            handler->OnKeyDown('J');
        else
            handler->OnKeyDown('j');
        break;
    case 0x25:
        if (shift)
            handler->OnKeyDown('K');
        else
            handler->OnKeyDown('k');
        break;
    case 0x26:
        if (shift)
            handler->OnKeyDown('L');
        else
            handler->OnKeyDown('l');
        break;
    case 0x32:
        if (shift)
            handler->OnKeyDown('M');
        else
            handler->OnKeyDown('m');
        break;
    case 0x31:
        if (shift)
            handler->OnKeyDown('N');
        else
            handler->OnKeyDown('n');
        break;
    case 0x18:
        if (shift)
            handler->OnKeyDown('O');
        else
            handler->OnKeyDown('o');
        break;
    case 0x19:
        if (shift)
            handler->OnKeyDown('P');
        else
            handler->OnKeyDown('p');
        break;
    case 0x10:
        if (shift)
            handler->OnKeyDown('Q');
        else
            handler->OnKeyDown('q');
        break;
    case 0x13:
        if (shift)
            handler->OnKeyDown('R');
        else
            handler->OnKeyDown('r');
        break;
    case 0x1F:
        if (shift)
            handler->OnKeyDown('S');
        else
            handler->OnKeyDown('s');
        break;
    case 0x14:
        if (shift)
            handler->OnKeyDown('T');
        else
            handler->OnKeyDown('t');
        break;
    case 0x16:
        if (shift)
            handler->OnKeyDown('U');
        else
            handler->OnKeyDown('u');
        break;
    case 0x2F:
        if (shift)
            handler->OnKeyDown('V');
        else
            handler->OnKeyDown('v');
        break;
    case 0x11:
        if (shift)
            handler->OnKeyDown('W');
        else
            handler->OnKeyDown('w');
        break;
    case 0x2D:
        if (shift)
            handler->OnKeyDown('X');
        else
            handler->OnKeyDown('x');
        break;
    case 0x15:
        if (shift)
            handler->OnKeyDown('Y');
        else
            handler->OnKeyDown('y');
        break;
    case 0x2C:
        if (shift)
            handler->OnKeyDown('Z');
        else
            handler->OnKeyDown('z');
        break;
    case 0x1A:
        handler->OnKeyDown('\n');
        simpleShell('\n', this);
        break;

    case 0x2A:
    case 0x36:
        shift = true;
        break;
    case 0xAA:
    case 0xB6:
        shift = false;
        break;
    default:
        if (key < 0x80)
        {
            printf(" keyboard0x");
            printfHex(key);
        }
        break;
    }

    return esp;
}

// 它执行一系列操作来初始化键盘控制器，包括清空键盘缓冲区和启用键盘中断
void KeyboardDriver::Activate()
{
    while (commandPort.Read() & 0x1)
        dataPort.Read();
    commandPort.Write(0xAE);
    commandPort.Write(0x20);
    uint8_t status = (dataPort.Read() | 0x01) & ~0x10;
    commandPort.Write(0x60);
    dataPort.Write(status);

    dataPort.Write(0xF4); // clean buffer
}