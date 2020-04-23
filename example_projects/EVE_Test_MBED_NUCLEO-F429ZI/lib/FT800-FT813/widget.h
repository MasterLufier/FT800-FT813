/*
 * @file widget.h
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
#ifndef WIDGET_H
#define WIDGET_H

#include <map>
#include <ft8xx.h>
#include <colors.h>

namespace FTGUI {
class Widget
{
public:
    Widget(Widget * parent);

    void addWidget(Widget * widget);

    const Widget * getWidget(string name) const;

    void removeWidget(string name);

    virtual ~Widget();

    virtual void show();
    virtual void hide();
    bool visible() const;

protected:
    virtual void render();
    string m_name;

    Widget * m_parent;
    Theme * m_theme;
    FT8xx * m_driver;

    ScreenOrientation m_orientation{};
    std::map<string, Widget *> m_container;
    bool m_visible{false};

    uint16_t x{0}, y{0}, z{0}, width{EVE_HSIZE}, height{EVE_VSIZE};
};
}

#endif // WIDGET_H
