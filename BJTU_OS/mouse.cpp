#include "mouse.h"

void printf(const char *);
void printfHex(const uint8_t );  // 用于以十六进制格式输出一个字节的数据

// 因为是虚函数所以现在不用实现
MouseEventHandler::MouseEventHandler() { }
void MouseEventHandler::OnMouseDown(uint8_t button) { }
void MouseEventHandler::OnMouseUp(uint8_t button) { }
void MouseEventHandler::OnMouseMove(uint16_t xoffset, uint16_t yoffset) { }


MouseDriver::MouseDriver(InterruptManager * manager, MouseEventHandler * handler)
    : InterruptHandler(0x2C, manager), dataPort(0x60), commandPort(0x64), offset(0), buttons(0), handler(handler)
{

}

MouseDriver::~MouseDriver()
{

}

/*首先，从命令端口读取鼠标的状态并保存在status变量中。
如果状态的第5位（0x20）为0，或者事件处理器对象handler为空指针，则直接返回原始的堆栈指针esp，不进行处理。
否则，从数据端口读取鼠标的输入数据，并将其存储在buffer数组的当前偏移位置offset处。
然后，更新offset的值，使其指向下一个位置。这样，buffer数组会循环使用，最多存储3个鼠标输入数据。
当offset的值等于0时，表示buffer数组已经存满了3个鼠标输入数据。在这种情况下，我们可以处理这些数据并触发相应的鼠标事件。
首先，检查buffer数组的第2个和第3个元素是否为零。
如果它们不都为零，说明鼠标发生了移动，我们调用事件处理器对象的OnMouseMove()函数，
并将鼠标移动的水平偏移量和垂直偏移量作为参数传递给它。
这里的水平偏移量是buffer[1]，垂直偏移量是-buffer[2]。注意，为了将垂直偏移量转换为正确的方向，我们将其取反。
接下来，我们通过循环遍历buffer数组的第1个元素的每个位来检测鼠标按键事件。
在循环中，我们使用位运算来比较buffer数组的第1个元素和buttons变量的对应位。
如果它们不相等，说明鼠标按键状态发生了变化。
对于每个不相等的位，我们首先检查buttons变量的相应位是否为1。
如果是，说明鼠标按键被释放，我们调用事件处理器对象的OnMouseUp()函数，并将对应的按键编号作为参数传递给它。
如果buttons变量的相应位是0，说明鼠标按键被按下，我们调用事件处理器对象的OnMouseDown()函数，并将对应的按键编号作为参数传递给它。
最后，我们更新buttons变量的值为buffer数组的第1个元素的值，以便下一次中断处理时可以正确比较按键状态。
最后，返回更新后的堆栈指针esp。*/
uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t status = commandPort.Read();
    if (!(status & 0x20) || handler == nullptr)
        return esp;

    buffer[offset] = dataPort.Read();
    offset = (offset + 1) % 3;

    if (offset == 0)
    {
        if (buffer[1] != 0 || buffer[2] != 0)
            handler ->OnMouseMove(buffer[1], -buffer[2]);
        for (uint8_t i = 0; i < 3; i ++)
        {
            if (buffer[0] & (i << i) != (buttons & (i << i)))
            {
                if (buttons & (1 << i))
                    handler ->OnMouseUp(i + 1);
                else
                    handler ->OnMouseDown(i + 1);
            }
        }
        buttons = buffer[0];
    }

    return esp;
}
/*首先，向命令端口写入0xA8，以启用鼠标中断。
接下来，向命令端口写入0x20，以读取并保存鼠标控制器的状态。
然后，从数据端口读取鼠标控制器的状态，并将其与0x02进行按位或运算，再与0x20进行按位取反运算，最后将结果保存在status变量中。
这样做是为了清除状态中的第5位，以便启用鼠标中断。
接着，向命令端口写入0x60，以将更新后的状态值写入鼠标控制器。
接下来，向命令端口写入0xD4，以选择鼠标数据端口。
然后，向数据端口写入0xF4，以清空鼠标的缓冲区，确保我们处理的是最新的鼠标数据。
最后，从数据端口读取一个字节，以确保鼠标控制器已经接受了我们的命令。*/
void MouseDriver::Activate()
{
    commandPort.Write(0xA8);
    commandPort.Write(0x20);
    uint8_t status = (dataPort.Read() | 0x02) & ~0x20;
    commandPort.Write(0x60);
    dataPort.Write(status);

    commandPort.Write(0xD4);
    dataPort.Write(0xF4); //clean buffer
    dataPort.Read();
}

