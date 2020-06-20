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
Widget::Widget(Widget * parent, bool modal) :
    Widget(0, 0, 0, 0, parent, modal)
{
}

Widget::Widget(int32_t  x,
               int32_t  y,
               uint16_t width,
               uint16_t height,
               Widget * parent,
               bool     modal) :
    m_modal(modal),
    m_visible(parent != this ? parent->visible() : false),
    m_x(x),
    m_y(y),
    m_width(width),
    m_height(height)
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
        m_parent->removeWidget(this);
        parent->addWidget(this);
    }
}

void Widget::addWidget(Widget * widget)
{
    widget->setParent(this);
    //Prevent adding self to child
    if(widget == this)
        return;
    m_container.push_back(widget);
}

void Widget::removeWidget(Widget * widget)
{
    m_container.erase(
        std::remove(m_container.begin(),
                    m_container.end(),
                    widget),
        m_container.end());
}

Widget::~Widget()
{
    //    if(m_onPressed)
    delete m_onPressed;
    //    if(m_onChanged)
    delete m_onChanged;
    //    if(m_onReleased)
    delete m_onReleased;
    for(auto w : m_container)
    {
        delete w;
    }
}

void Widget::show()
{
    if(checkPositionInScreen() == false)
        return;
    for(const auto & w : m_container)
    {
        if(w->visible() != false)
            w->show();
    }
    //    debug("%s : %i, %i, %u, %u \n", m_name.c_str(), m_x, m_y, m_width, m_height);
    m_visible = true;
}

void Widget::hide()
{
    m_visible = false;
    for(const auto & w : m_container)
    {
        w->hide();
    }
}

void Widget::update()
{
    m_parent->update();
}

void Widget::animationStarted(void *   value,
                              uint32_t duration,
                              uint8_t  delay)
{
    if(value)
    {
        queue()->call_in(
            (duration),
            [&, value]() {
                m_animationBlock.erase(
                    std::find(m_animationBlock.begin(),
                              m_animationBlock.end(),
                              value));
            });
    }
    if(m_parent != this)
    {
        m_parent->animationStarted(nullptr, duration, delay);
    }
}

void Widget::setGeometry(int32_t  x,
                         int32_t  y,
                         uint16_t width,
                         uint16_t height)
{
    m_x      = x;
    m_y      = y;
    m_width  = width;
    m_height = height;
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
    m_parent      = parent;
    m_driver      = m_parent->driver();
    m_orientation = m_parent->orientation();
    m_theme       = m_parent->theme();
    m_queue       = m_parent->queue();
}

FT8xx * Widget::driver() const
{
    return m_driver;
}

ScreenOrientation Widget::orientation() const
{
    return m_orientation;
}

void Widget::setTheme(Theme * theme)
{
    m_theme = theme;
}

bool Widget::checkPositionInScreen()
{
    //Check is widget is in screen
    if(absX() > EVE_HSIZE
       || absY() > EVE_VSIZE
       || absX() + width() < 0
       || absY() + height() < 0)
    {
        return false;
    }
    return true;
}

void Widget::animation(int32_t *       value,
                       int32_t         from,
                       int32_t         to,
                       uint32_t        duration,
                       FT8xx::FadeType fadeType,
                       uint8_t         delay)
{
    if(std::find(
           m_animationBlock.begin(),
           m_animationBlock.end(),
           value)
       != m_animationBlock.end())
    {
        debug("Blocked\n");
        return;
    }
    m_animationBlock.push_back(value);
    m_driver->animate(
        value,
        from,
        to,
        duration,
        fadeType,
        delay);
    animationStarted(value, duration, delay);
}

bool Widget::modal() const
{
    return m_modal;
}

EventQueue * Widget::queue() const
{
    return m_queue;
}

int32_t Widget::absX() const
{
    int32_t x = m_x;
    if(m_parent != this && m_parent)
    {
        x += m_parent->absX();
    }
    return x;
}

int32_t Widget::absY() const
{
    int32_t y = m_y;
    if(m_parent != this && m_parent)
    {
        y += m_parent->absY();
    }
    return y;
}

bool Widget::visible() const
{
    return m_visible;
}

void Widget::setVisible(bool visible)
{
    m_visible = visible;
    for(const auto & w : m_container)
    {
        w->setVisible(visible);
    }
    //    update();
}

bool Widget::toggleVisible()
{
    setVisible(!m_visible);
    update();
    return m_visible;
}

void Widget::setHeight(uint16_t height)
{
    m_height = height;
}

bool Widget::touchPressed(int16_t x,
                          int16_t y)
{
    if(m_visible == false)
        return false;
    if(x > absX()
       && x < absX() + m_width
       && y > absY()
       && y < absY() + m_height)
    {
        if(m_onPressed)
            m_onPressed->operator()(x, y);
        for(const auto & w : m_container)
        {
            w->touchPressed(x, y);
        }
        //        debug("%s pressed %i:%i\n", m_name.c_str(), x, y);
        return true;
    }
    return false;
}

bool Widget::touchChanged(int16_t         x,
                          int16_t         y,
                          const int16_t * accelerationX,
                          const int16_t * accelerationY)
{
    if(m_visible == false)
        return false;
    if(x > absX()
       && x < absX() + m_width
       && y > absY()
       && y < absY() + m_height)
    {
        if(m_onChanged)
            m_onChanged->operator()(x, y);
        for(const auto & w : m_container)
        {
            w->touchChanged(x, y, accelerationX, accelerationY);
        }
        //            debug("%s changed %i:%i\n", m_name.c_str(), x, y);
        return true;
    }
    return false;
}

bool Widget::touchReleased(int16_t x,
                           int16_t y,
                           int16_t accelerationX,
                           int16_t accelerationY)
{
    if(m_visible == false)
        return false;
    if(x > absX()
       && x < absX() + m_width
       && y > absY()
       && y < absY() + m_height)
    {
        if(m_onReleased)
            m_onReleased->operator()(x, y);
        for(const auto & w : m_container)
        {
            w->touchReleased(x, y, accelerationX, accelerationY);
        }
        //            debug("%s released %i:%i\n", m_name.c_str(), x, y);
        return true;
    }
    return false;
}

void Widget::setWidth(uint16_t width)
{
    m_width = width;
}

void Widget::setY(int32_t y)
{
    m_y = y;
}

void Widget::setX(int32_t x)
{
    m_x = x;
}

void Widget::setZ(uint16_t z)
{
    m_z = z;
    if(m_parent)
        std::sort(m_parent->m_container.begin(), m_parent->m_container.end(), [](const Widget * w1, const Widget * w2) {
            return w1->z() < w2->z();
        });
}

uint16_t Widget::z() const
{
    return m_z;
}

int32_t Widget::x() const
{
    return m_x;
}

int32_t Widget::y() const
{
    return m_y;
}

uint16_t Widget::height() const
{
    return m_height;
}

uint16_t Widget::width() const
{
    return m_width;
}

}    // namespace FTGUI
