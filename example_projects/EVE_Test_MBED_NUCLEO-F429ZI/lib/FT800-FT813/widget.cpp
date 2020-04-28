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

Widget::Widget(string name, Widget *parent) :
      m_name(name)
{
    if(parent)
    {
        //if parent is empty
        if(m_parent == nullptr)
        {
            parent->addWidget(this);
            return;
        }
        //if parent is same
        if(parent == m_parent)
            return;
        //if change parent needed
        m_parent->removeWidget(this->name());
        parent->addWidget(this);
    }
}

void Widget::addWidget(Widget *widget)
{
    widget->setParent(this);
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
    for(const auto & w : m_container)
    {
        w.second->show();
    }
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

void Widget::setGeometry(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
}

const string & Widget::name() const
{
    return m_name;
}

Theme * Widget::theme() const
{
    return m_theme;
}

Widget * Widget::parent() const
{
    return m_parent;
}

void Widget::setParent(Widget * parent)
{
    m_parent = parent;
    m_driver = m_parent->driver();
    m_orientation = m_parent->orientation();
    m_theme = m_parent->theme();
}

FT8xx *Widget::driver() const
{
    return m_driver;
}

ScreenOrientation Widget::orientation() const
{
    return m_orientation;
}

void Widget::setTheme(Theme *theme)
{
    m_theme = theme;
}

Widget::Widget(string name) : m_name(name)
{
    m_parent = this;
}

}
