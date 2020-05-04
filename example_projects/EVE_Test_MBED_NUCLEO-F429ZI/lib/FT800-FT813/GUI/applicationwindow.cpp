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

namespace FTGUI
{
ApplicationWindow::ApplicationWindow(Theme *           theme,
                                     ScreenOrientation screenOrientation,
                                     FT8xx *           driver) :
    Widget(0, 0, EVE_HSIZE, EVE_VSIZE, this)
{
    m_name = "ApplicationWidow";
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

    //Initialize memory
    m_driver->ramGInit();

    //Initialize Touchscreen
    m_driver->touchCalibrate();
    //Switch on Backlight
    m_driver->backlightFade(0, 128);
}

ApplicationWindow::~ApplicationWindow()
{
    delete m_theme;
    delete m_driver;
}

void ApplicationWindow::show()
{
    if(m_renderLock == false)
        m_renderLock = true;
    else
        return;

    if(m_visible != true)
        setVisible(true);
    EVE_cmd_dl(CMD_DLSTART);
    EVE_cmd_dl(DL_CLEAR_RGB | m_theme->background().hex());
    EVE_cmd_dl(CLEAR(1, 1, 1));
    Widget::show();
    EVE_cmd_dl(DL_DISPLAY);
    EVE_cmd_dl(CMD_SWAP);
    EVE_cmd_execute();
    m_renderLock = false;
}

void ApplicationWindow::hide()
{
    Widget::hide();
}
}    // namespace FTGUI
