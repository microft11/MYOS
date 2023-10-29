#include "keyboard.h"

void printf(const char *);
void printfHex(const uint8_t );

KeyboardEventHandler::KeyboardEventHandler() {}

KeyboardDriver::KeyboardDriver(InterruptManager * manager, KeyboardEventHandler * handler)
    : InterruptHandler(0x21, manager), dataPort(0x60), commandPort(0x64), handler(handler)
{

}

KeyboardDriver::~KeyboardDriver(){}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t key = dataPort.Read();
    if (handler == 0)
        return esp;
    static bool shift = false;

    char msg[2] = {'\0'};
    switch(key) 
    {
    case 0x45: break;
    case 0x1E:
        if (shift) handler ->OnKeyDown('A');
        else handler ->OnKeyDown('a');
        printf(msg);
        break;
    case 0x1F:
        if (shift) handler ->OnKeyDown('S');
        else handler ->OnKeyDown('s');
        printf(msg);
        break;
    case 0x20:
        msg[0] = 'd';
        printf(msg);
        break;
    case 0x21:
        msg[0] = 'f';
        printf(msg);
        break;
    case 0x2A: case 0x36: shift = true; break;
    case 0xAA: case 0xB6: shift = false; break;
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

void KeyboardDriver::Activate()
{
    while (commandPort.Read() & 0x1)
        dataPort.Read();
    commandPort.Write(0xAE);
    commandPort.Write(0x20);
    uint8_t status = (dataPort.Read() | 0x01) & ~0x10;
    commandPort.Write(0x60);
    dataPort.Write(status);

    dataPort.Write(0xF4); //clean buffer
}