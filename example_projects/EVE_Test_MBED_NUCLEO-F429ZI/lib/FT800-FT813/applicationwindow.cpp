/*
 * @file applicationwindow.cpp
 * is part of FTGUI Project
 *
 * Copyright (c) 2020 Mikhail Ivanov <masluf@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include "applicationwindow.h"

namespace FTGUI {
ApplicationWindow::ApplicationWindow(FT8xx * driver,
                                     ScreenOrientation screenOrientation,
                                     Theme * theme) :
      Widget("ApplicationWindow")
{
    if(driver)
        m_driver = driver;
    else
        m_driver = new FT8xx(EVE_SPI_MOSI,
                             EVE_SPI_MISO,
                             EVE_SPI_CLK,
                             EVE_SPI_SSEL,
                             EVE_PD,
                             EVE_INTRPT);
    m_orientation = screenOrientation;
    if(theme)
        m_theme = theme;
    else
        m_theme = new Theme();

    //Initialize Touchscreen
    m_driver->touchCalibrate();
    m_driver->backlightFade(0,128);
}

ApplicationWindow::~ApplicationWindow()
{
    delete m_theme;
    delete m_driver;
}

void ApplicationWindow::show()
{
    //        EVE_start_cmd_burst();
    EVE_cmd_dl(CMD_DLSTART);
    EVE_cmd_dl(DL_CLEAR_RGB | m_theme->background().hex());
    EVE_cmd_dl(CLEAR(1, 1, 1));
//    EVE_cmd_dl(CMD_SWAP);
//    EVE_cmd_execute();
    Widget::show();
    EVE_cmd_dl(DL_DISPLAY);
    EVE_cmd_dl(CMD_SWAP);
    //        EVE_end_cmd_burst();
    EVE_cmd_execute();
    //EVE_start_cmd_burst();
    //    EVE_cmd_dl(CMD_DLSTART);
    //    EVE_cmd_dl(DL_CLEAR_RGB | m_theme->background().hex());
    //    EVE_cmd_dl(CLEAR(1, 1, 1));
    //    EVE_cmd_dl(BEGIN(EVE_POINTS));
    //    for (int i = 0; i < 165; i++) {
    //        EVE_cmd_dl(DL_COLOR_RGB | (rand() %0x00ffffff));
    //        EVE_cmd_dl(COLOR_A(rand() %256));
    //        EVE_cmd_point(rand() %480, rand()%272, rand() %(50));
    //    }
    //    EVE_cmd_dl(DL_DISPLAY);
    //    EVE_cmd_dl(CMD_SWAP);
    //    EVE_cmd_execute();
}

void ApplicationWindow::hide()
{
    Widget::hide();
}
}
