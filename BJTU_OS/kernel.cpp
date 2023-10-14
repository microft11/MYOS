#include "types.h"
#include "gdt.h"
void printf(const int8_t * str) 
{
    static int16_t * VideoMemory = (short*)0xb8000;
    static int8_t x = 0, y = 0;
    for (int32_t i = 0; str[i] != '\0'; i ++)
    {
        
        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | str[i];
        x ++;
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


        while (1)
            ;
    }