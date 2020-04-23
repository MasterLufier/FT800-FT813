/*
 * @file widget.cpp
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
#include "widget.h"

namespace FTGUI
{

Widget::Widget(Widget *parent) : m_parent(parent)
{
    if(parent)
    {
        m_theme = m_parent->m_theme;
        m_orientation = parent->m_orientation;
        m_driver = parent->m_driver;
    }
}

void Widget::addWidget(Widget *widget)
{
    widget->m_parent = this;
    m_container.emplace(widget->m_name, widget);
}

const Widget *Widget::getWidget(string name) const
{
    auto it = m_container.find(name);
    return it->second;
}

void Widget::removeWidget(string name)
{
    auto it = m_container.find(name);
    if(it != m_container.end())
    {
        it = m_container.erase(it);
        delete it->second;
    }
}

Widget::~Widget()
{
    for(auto & w : m_container)
        delete w.second;
}

void Widget::show()
{
    m_visible = true;
}

void Widget::hide()
{
    m_visible = false;
}

bool Widget::visible() const
{
    return m_visible;
}

void Widget::render()
{

}

}
