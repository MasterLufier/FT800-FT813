/*
 * @file page.cpp
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
#include "page.h"

namespace FTGUI
{
Page::Page(Widget * parent) :
    Widget(parent)
{
    m_name        = "Page";
    m_background  = new Rectangle(this);
    m_contentItem = new Widget(this);
    m_background->setColor(m_theme->background());
    m_background->setBorderColor(m_theme->onPrimary());
    m_background->setBorderWidth(1);
    m_background->setRadius(5);

    //Fix bg and contentItem Geometry initialization
    setGeometry(m_x, m_y, m_width, m_height);
}

void Page::addWidget(Widget * widget)
{
    if(!m_background)
    {
        m_background = reinterpret_cast<Rectangle *>(widget);
        widget->setParent(this);
        m_container.push_back(widget);
        return;
    }
    if(!m_contentItem)
    {
        m_contentItem = widget;
        widget->setParent(this);
        m_container.push_back(widget);
        return;
    }
    m_contentItem->addWidget(widget);
    setGeometry(m_x, m_y, m_width, m_height);
}

void Page::setGeometry(int32_t  x,
                       int32_t  y,
                       uint16_t width,
                       uint16_t height)
{
    Widget::setGeometry(x, y, width, height);
    m_background->setGeometry(m_padding,
                              m_padding,
                              m_width - m_padding * 2,
                              m_height - m_padding * 2);

    m_contentItem->setGeometry(m_padding + m_margins,
                               m_padding + m_margins,
                               m_width - m_padding * 2 - m_margins * 2,
                               m_height - m_padding * 2 - m_margins * 2);
}

void Page::setWidth(uint16_t width)
{
    Widget::setWidth(width);
    m_background->setWidth(m_width - m_padding * 2);
    m_contentItem->setWidth(m_width - m_padding * 2 - m_margins * 2);
}

void Page::setHeight(uint16_t height)
{
    Widget::setHeight(height);
    m_background->setHeight(m_height - m_padding * 2);
    m_contentItem->setHeight(m_height - m_padding * 2 - m_margins * 2);
}
}    // namespace FTGUI
