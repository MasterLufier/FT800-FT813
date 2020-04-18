/**
@file    ft8xx.cpp
@brief   EVE - HMI solution library for based on FT-BT8xx chip
@version 0.1
@date    2020-04-19
@author  Mikhail Ivanov

@section LICENSE

    MIT License

        Copyright (c) 2020 Mikhail Ivanov

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
                                                              copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

       THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
        SOFTWARE.
            */
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
