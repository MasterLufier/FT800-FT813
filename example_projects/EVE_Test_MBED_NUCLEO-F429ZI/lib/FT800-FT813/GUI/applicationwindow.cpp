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
                                     ScreenOrientation screenOrientation) :
    Widget(0, 0, EVE_HSIZE, EVE_VSIZE, this)
{
    m_name = "ApplicationWidow";

    m_driver = new FT8xx(EVE_SPI_MOSI,
                         EVE_SPI_MISO,
                         EVE_SPI_CLK,
                         EVE_SPI_SSEL,
                         EVE_PD,
                         EVE_INTRPT);

    m_queue = new EventQueue(96 * EVENTS_EVENT_SIZE);
    m_thread.start(mbed::callback(m_queue, &EventQueue::dispatch_forever));
    m_orientation = screenOrientation;
    if(theme)
        m_theme = theme;
    else
        m_theme = new Theme();

    //Initialize memory
    m_driver->ramGInit();

    //Initialize Touchscreen
    m_driver->touchCalibrate();

    //set touch callbacks
    m_driver->attachTouchDetectedCallback([&](uint8_t) {
        FT8xx::CmdBuf_t xy = m_driver->touchXY();
        m_xFifo.push_back(xy.halfWord[1]);
        m_yFifo.push_back(xy.halfWord[0]);
        m_touchPressed = true;
    });

    m_driver->attachTouchConversionsCallback([&](uint8_t) {
        FT8xx::CmdBuf_t xy = m_driver->touchXY();
        //        debug("XY: %i:%i\n", xy.halfWord[1], xy.halfWord[0]);
        //NOTE: Check filtering parameters after assembly!!!
        if(xy.word == (int32_t)0x80008000)
        {
            m_queue->call([&,
                           x    = m_prevX,
                           y    = m_prevY,
                           accX = m_accelerationX,
                           accY = m_accelerationY]() {
                touchReleased(x, y, accX, accY);
            });
            m_xFifo.clear();
            m_yFifo.clear();
            m_accelerationTicker.detach();
            m_accelerationX = 0;
            m_accelerationY = 0;
            m_prevX         = 0;
            m_prevY         = 0;
            return;
        }

        m_xFifo.push_back(xy.halfWord[1]);
        m_yFifo.push_back(xy.halfWord[0]);

        if(m_xFifo.size() > 12)
        {
            std::sort(m_xFifo.begin(),
                      m_xFifo.end());
            std::sort(m_yFifo.begin(),
                      m_yFifo.end());

            m_xFifo.pop_front();
            m_yFifo.pop_front();
            m_xFifo.pop_front();
            m_yFifo.pop_front();
            m_xFifo.pop_front();
            m_yFifo.pop_front();

            m_xFifo.pop_back();
            m_yFifo.pop_back();
            m_xFifo.pop_back();
            m_yFifo.pop_back();
            m_xFifo.pop_back();
            m_yFifo.pop_back();

            if(m_touchPressed)
            {
                m_prevX = m_xFifo[3];
                m_prevY = m_yFifo[3];
                m_queue->call([&, x = m_prevX, y = m_prevY]() {
                    touchPressed(x, y);
                    m_accelerationTicker.attach(callback(this, &ApplicationWindow::deceleration), 0.1);
                });
                m_touchPressed = false;
                return;
            }

            //check if touch point changed
            if(abs(m_prevX - m_xFifo[3]) > 1
               || abs(m_prevY - m_yFifo[3]) > 1)
            {
                m_accelerationX = m_xFifo[3] - m_prevX;
                m_accelerationY = m_yFifo[3] - m_prevY;
                //                debug("prev XY: %i:%i .. %i:%i\n", m_xFifo[3], m_prevX, m_prevY, m_yFifo[3]);
                //                debug("Accl XY: %i:%i\n", m_accelerationX, m_accelerationY);

                m_prevX = m_xFifo[3];
                m_prevY = m_yFifo[3];
                m_queue->call([&, x = m_prevX, y = m_prevY]() {
                    touchChanged(x,
                                 y,
                                 &m_accelerationX,
                                 &m_accelerationY);
                });
            }
        }
    });
    //Switch on Backlight
    m_driver->backlightFade(0, 128);
}

ApplicationWindow::~ApplicationWindow()
{
    m_thread.terminate();
    for(auto w : m_modalContainer)
    {
        delete w;
    }
    delete m_theme;
    delete m_driver;
    delete m_queue;
}

void ApplicationWindow::show()
{
    if(m_renderLock == false)
        m_renderLock = true;
    else
        return;

    if(!m_modalContainer.empty())
    {
        for(const auto w : m_modalContainer)
        {
            if(w->visible())
            {
                if(m_animationCounter == 0 && !m_modalOpened)
                    w->prepare();
                m_modalOpened = true;
                m_driver->dlStart();
                m_driver->clearColorRGB(m_theme->background().hex());
                m_driver->clear();
                w->show();
                m_driver->swap();
                m_driver->execute();
                m_renderLock = false;
                return;
            }
        }
        m_modalOpened = false;
    }

    if(m_visible != true)
        setVisible(true);
    m_driver->dlStart();
    m_driver->clearColorRGB(m_theme->background().hex());
    m_driver->clear();
    Widget::show();
    m_driver->swap();
    m_driver->execute();
    m_renderLock = false;
}

void ApplicationWindow::hide()
{
    Widget::hide();
}

void ApplicationWindow::addWidget(Widget * widget)
{
    if(widget->modal())
    {
        widget->setParent(this);
        m_modalContainer.push_back(reinterpret_cast<ModalWidget *>(widget));
    }
    else
        Widget::addWidget(widget);
}

bool ApplicationWindow::touchPressed(int16_t x, int16_t y)
{
    if(m_modalOpened)
    {
        for(const auto & w : m_modalContainer)
        {
            w->touchPressed(x, y);
        }
    }
    else
    {
        Widget::touchPressed(x, y);
    }
}

bool ApplicationWindow::touchChanged(int16_t         x,
                                     int16_t         y,
                                     const int16_t * accelerationX,
                                     const int16_t * accelerationY)
{
    if(m_modalOpened)
    {
        for(const auto & w : m_modalContainer)
        {
            w->touchChanged(x, y, accelerationX, accelerationY);
        }
    }
    else
    {
        Widget::touchChanged(x, y, accelerationX, accelerationY);
    }
}

bool ApplicationWindow::touchReleased(int16_t x,
                                      int16_t y,
                                      int16_t accelerationX,
                                      int16_t accelerationY)
{
    if(m_modalOpened)
    {
        for(const auto & w : m_modalContainer)
        {
            w->touchReleased(x, y, accelerationX, accelerationY);
        }
    }
    else
    {
        Widget::touchReleased(x, y, accelerationX, accelerationY);
    }
}

void ApplicationWindow::animationStarted(void *   value,
                                         uint32_t duration,
                                         uint8_t  delay)
{
    if(m_animationCounter == 0)
    {
        m_updateEventId = queue()->call_every(delay, this, &ApplicationWindow::update);
    }

    queue()->call_in((duration + duration / 2), [&]() {
        if(--m_animationCounter == 0)
        {
            queue()->cancel(m_updateEventId);
        }
    });
    m_animationCounter++;
}

void ApplicationWindow::update()
{
    if(m_visible == true)
        show();
    else
        hide();
}
}    // namespace FTGUI
