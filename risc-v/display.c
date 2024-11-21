
int main()
{
    volatile char *VGA = (volatile char*) 0x08000000;

    for (int i = 0; i < 320*240; i++)
    VGA[i] = i / 320;

    unsigned int y_ofs = 0;

    volatile int *VGA_CTRL = (volatile int*) 0x04000100;
    while (1)
        {
            *(VGA_CTRL+1) = (unsigned int) (VGA+y_ofs*320);
            *(VGA_CTRL+0) = 0;
            y_ofs = (y_ofs +1) % 240;
            for (int i = 0; i < 1000000; i++)
            asm volatile ("nop");

        }

}