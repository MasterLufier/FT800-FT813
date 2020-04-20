#include <mbed.h>
#include <ft8xx.h>

Serial pc(USBTX, USBRX);

extern void loadTestGraphic()
{
    //Load start graphics from BT81X Series Programming Guide
    EVE_memWrite32(EVE_RAM_DL + 0, CLEAR(1, 1, 1)); // clear screen
    EVE_memWrite32(EVE_RAM_DL + 4, BEGIN(EVE_BITMAPS)); // start drawing bitmaps
    EVE_memWrite32(EVE_RAM_DL + 8, VERTEX2II(220, 110, 31, 'T')); // ascii T in font 31
    EVE_memWrite32(EVE_RAM_DL + 12,VERTEX2II(244, 110, 31, 'E')); // ascii E
    EVE_memWrite32(EVE_RAM_DL + 16, VERTEX2II(270, 110, 31, 'X')); // ascii X
    EVE_memWrite32(EVE_RAM_DL + 20, VERTEX2II(299, 110, 31, 'T')); // ascii T
    EVE_memWrite32(EVE_RAM_DL + 24, END());
    EVE_memWrite32(EVE_RAM_DL + 28, COLOR_RGB(160, 22, 22)); // change colour to red
    EVE_memWrite32(EVE_RAM_DL + 32, POINT_SIZE(320)); // set point size to 20 pixels in radius
    EVE_memWrite32(EVE_RAM_DL + 36, BEGIN(EVE_POINTS)); // start drawing points
    EVE_memWrite32(EVE_RAM_DL + 40, VERTEX2II(192, 133, 0, 0)); // red point
    EVE_memWrite32(EVE_RAM_DL + 44, END());
    EVE_memWrite32(EVE_RAM_DL + 48, DL_DISPLAY); // display the image
    EVE_memWrite32(REG_DLSWAP, EVE_DLSWAP_FRAME);//display list swap
}

DigitalOut green(LED1);
int main()
{
    pc.baud(115200);
    FT8xx m_screen(PF_9, PF_8, PF_7, PF_12, PB_2, PD_15);
    while (1) {
        ThisThread::sleep_for(1000);
        green = !green;
        loadTestGraphic();
    }
    return 0;
}


