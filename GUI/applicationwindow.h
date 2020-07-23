/*
 * @file applicationwindow.h
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
#ifndef APPLICATIONWINDOW_H
#define APPLICATIONWINDOW_H

#include <Containers/dialog.h>
#include <deque>
#include <graphics.h>
#include <widget.h>

namespace FTGUI
{
class ApplicationWindow : public Widget,
                          private NonCopyable<ApplicationWindow>
{
public:
    ApplicationWindow(Theme *           theme             = nullptr,
                      ScreenOrientation screenOrientation = Horizontal);
    ~ApplicationWindow() override;

    void show() override;
    void hide() override;

    void addWidget(Widget * widget) override;

protected:
    bool touchPressed(int16_t x, int16_t y) override;
    bool touchChanged(int16_t x, int16_t y, const int16_t * accelerationX, const int16_t * accelerationY) override;
    bool touchReleased(int16_t x, int16_t y, int16_t accelerationX, int16_t accelerationY) override;

    void animationStarted(void *   value,
                          uint32_t duration = Duration,
                          uint8_t  delay    = Delay) override;
    void update() override;

    LowPowerTicker m_accelerationTicker;

    void deceleration()
    {
        if(m_accelerationX < 0)
            m_accelerationX++;
        else if(m_accelerationX > 0)
            m_accelerationX--;

        if(m_accelerationY < 0)
            m_accelerationY++;
        else if(m_accelerationY > 0)
            m_accelerationY--;
    }

    bool m_renderLock{false},
        m_touchPressed{false},
        m_modalOpened{false};
    int16_t m_prevX{0},
        m_prevY{0},
        m_accelerationX{0},
        m_accelerationY{0};

    std::vector<ModalWidget *> m_modalContainer;

    std::deque<int16_t> m_xFifo;
    std::deque<int16_t> m_yFifo;
    uint8_t             m_animationCounter{0};
    int32_t             m_updateEventId{0};
    Thread              m_thread{osPriorityNormal, 2048, nullptr, "GUIThread"};
};
}    // namespace FTGUI

#endif    // APPLICATIONWINDOW_H
