/*
 * @file page.h
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
#ifndef PAGE_H
#define PAGE_H

#include <graphics.h>

namespace FTGUI
{
class Page : public Widget
{
public:
    Page(Widget * parent = nullptr);

    void addWidget(Widget * widget) override;
    void setGeometry(int32_t  x,
                     int32_t  y,
                     uint16_t width,
                     uint16_t height) override;
    void setWidth(uint16_t width) override;
    void setHeight(uint16_t height) override;

protected:
    uint8_t     m_padding{5};
    uint8_t     m_margins{3};
    Rectangle * m_background{nullptr};
    Widget *    m_contentItem{nullptr};
};
}    // namespace FTGUI
#endif    // PAGE_H
