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
    Widget(string name, Widget * parent);

    void addWidget(Widget * widget);

    const Widget * getWidget(string name) const;

    void removeWidget(string name);

    virtual ~Widget();

    virtual void show();
    virtual void hide();
    bool visible() const;

    void setGeometry(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

    const string & name() const;

    Theme * theme() const;

    Widget *parent() const;
    void setParent(Widget *parent);

    FT8xx *driver() const;
    ScreenOrientation orientation() const;

    void setTheme(Theme *theme);

protected:
    //Special constructor for root object
    Widget(string name);
    string m_name;

    Widget * m_parent{nullptr};
    Theme * m_theme{nullptr};
    FT8xx * m_driver{nullptr};

    ScreenOrientation m_orientation{};
    std::map<string, Widget *> m_container;
    bool m_visible{false};

    uint16_t m_x{0}, m_y{0}, m_z{0}, m_width{EVE_HSIZE}, m_height{EVE_VSIZE};
};
}

#endif // WIDGET_H
