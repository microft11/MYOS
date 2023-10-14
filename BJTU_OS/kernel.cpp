#include "types.h"
#include "gdt.h"
#include "interrupts.h"
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
        GlobalDescriptorTable gdt;
        printf("hello OS world!\n");
        printf("hello OS world!\n");

        InterruptManger interrupts(&gdt);
        interrupts.Activate();

        while (1)
            ;
    }