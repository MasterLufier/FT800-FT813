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

#include <graphics.h>
#include <widget.h>

namespace FTGUI
{
class ApplicationWindow : public Widget,
                          private NonCopyable<ApplicationWindow>
{
public:
    ApplicationWindow(Theme *           theme             = nullptr,
                      ScreenOrientation screenOrientation = Horizontal,
                      FT8xx *           driver            = nullptr);
    ~ApplicationWindow() override;

    void show() override;
    void hide() override;

protected:
    bool m_renderLock{false};
};
}    // namespace FTGUI

#endif    // APPLICATIONWINDOW_H
