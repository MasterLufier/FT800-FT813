#include "ft8xx.h"

FT8xx::FT8xx(PinName mosi, PinName miso, PinName sclk, PinName ssel, PinName pd, PinName interrupt)
{
    m_hal = EVE_HAL::instance(mosi, miso, sclk, ssel, pd, interrupt);
    EVE_init();
    m_hal->setSPIfrequency(EVE_HAL::F_20M);
    loadTestGraphic();
}

FT8xx::~FT8xx()
{
    delete m_hal;
}

void FT8xx::loadTestGraphic()
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
