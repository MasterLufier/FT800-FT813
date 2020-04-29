/*!
 * @file rectangle.cpp
 * is part of FTGUI Project
 *
 * @copyright (c) 2020 Mikhail Ivanov <masluf@gmail.com>
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
#include "graphics.h"

namespace FTGUI
{
Rectangle::Rectangle(string   name,
                     Widget * parent) :
    Rectangle(0, 0, 0, 0, name, parent) {}

Rectangle::Rectangle(uint16_t x,
                     uint16_t y,
                     uint16_t width,
                     uint16_t height,
                     string   name,
                     Widget * parent) :
    Widget(name, parent)
{
    m_x      = x;
    m_y      = y;
    m_width  = width;
    m_height = height;
}

void Rectangle::show()
{
    //Draw shadow
    if(m_z != 0)
    {
        EVE_cmd_dl(DL_COLOR_RGB | 0x040404);
        EVE_cmd_dl(BLEND_FUNC(EVE_SRC_ALPHA, EVE_ONE_MINUS_SRC_ALPHA));
        EVE_cmd_dl(LINE_WIDTH(24 + ((m_radius - 1) * 16)));
        EVE_cmd_dl(BEGIN(EVE_RECTS));
        float shadow = (m_color.a() < 48 ? m_color.a() : 48);
        for(uint8_t i = m_z; i > 0; --i)
        {
            EVE_cmd_dl(COLOR_A(static_cast<uint8_t>(shadow / i)));
            //Draw ambient light shadow
            m_driver->drawVertexPointF(parent()->x() + m_x + m_radius - i,
                                       parent()->y() + m_y + m_radius - i);
            m_driver->drawVertexPointF(parent()->x() + m_x + m_width - m_radius + i,
                                       parent()->y() + m_y + m_height - m_radius + i);
            //Draw key light shadow
            m_driver->drawVertexPointF(parent()->x() + m_x + m_radius,
                                       parent()->y() + m_y + m_radius);
            m_driver->drawVertexPointF(parent()->x() + m_x + m_width - m_radius,
                                       parent()->y() + m_y + m_height - m_radius + i);
        }
        //        EVE_cmd_dl(END());
        //        EVE_cmd_dl(BEGIN(EVE_RECTS));
    }
    else
    {
        EVE_cmd_dl(BEGIN(EVE_RECTS));
    }
    //Draw border if provided
    if(m_borderWidth != 0)
    {
        EVE_cmd_dl(DL_COLOR_RGB | (m_borderColor.hex()));
        //Fix subpixel line opacity
        if(m_radius == 0)
        {
            if(m_borderColor.a() == 255)
            {
                EVE_cmd_dl(BLEND_FUNC(EVE_ONE, EVE_ZERO));
                EVE_cmd_dl(LINE_WIDTH(1));
            }
            else
            {
                EVE_cmd_dl(BLEND_FUNC(EVE_SRC_ALPHA, EVE_ONE_MINUS_SRC_ALPHA));
                EVE_cmd_dl(COLOR_A(m_borderColor.a()));
                EVE_cmd_dl(LINE_WIDTH(8));
            }
            m_driver->drawVertexPointF(parent()->x() + m_x,
                                       parent()->y() + m_y);
            m_driver->drawVertexPointF(parent()->x() + m_x + m_width,
                                       parent()->y() + m_y + m_height);
        }
        else
        {
            EVE_cmd_dl(COLOR_A(m_borderColor.a()));
            EVE_cmd_dl(BLEND_FUNC(EVE_SRC_ALPHA, EVE_ONE_MINUS_SRC_ALPHA));
            EVE_cmd_dl(LINE_WIDTH(24 + ((m_radius - 1) * 16)));
            m_driver->drawVertexPointF(parent()->x() + m_x + m_radius,
                                       parent()->y() + m_y + m_radius);
            m_driver->drawVertexPointF(parent()->x() + m_x + m_width - m_radius,
                                       parent()->y() + m_y + m_height - m_radius);
        }
    }
    //Draw body
    EVE_cmd_dl(DL_COLOR_RGB | (m_color.hex()));
    //Fix subpixel line opacity
    if(m_radius == 0)
    {
        if(m_color.a() == 255)
        {
            EVE_cmd_dl(BLEND_FUNC(EVE_ONE, EVE_ZERO));
            EVE_cmd_dl(LINE_WIDTH(1));
        }
        else
        {
            EVE_cmd_dl(COLOR_A(m_color.a()));
            EVE_cmd_dl(BLEND_FUNC(EVE_SRC_ALPHA, EVE_ONE_MINUS_SRC_ALPHA));
            EVE_cmd_dl(LINE_WIDTH(8));
        }
    }
    else
    {
        EVE_cmd_dl(COLOR_A(m_borderColor.a()));
        EVE_cmd_dl(BLEND_FUNC(EVE_SRC_ALPHA, EVE_ONE_MINUS_SRC_ALPHA));
        EVE_cmd_dl(LINE_WIDTH(24 + ((m_radius - 1) * 16)));
    }

    m_driver->drawVertexPointF(parent()->x() + m_x + m_borderWidth + m_radius,
                               parent()->y() + m_y + m_borderWidth + m_radius);

    m_driver->drawVertexPointF(parent()->x() + m_x + m_width - m_borderWidth - m_radius,
                               parent()->y() + m_y + m_height - m_borderWidth - m_radius);

    EVE_cmd_dl(END());

    Widget::show();
}

Rectangle & Rectangle::setGeometry(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    Widget::setGeometry(x, y, width, height);
    return *this;
}

Rectangle & Rectangle::setX(uint16_t x)
{
    Widget::setX(x);
    return *this;
}

Rectangle & Rectangle::setY(uint16_t y)
{
    Widget::setY(y);
    return *this;
}

Rectangle & Rectangle::setZ(uint16_t z)
{
    Widget::setZ(z);
    return *this;
}

Rectangle & Rectangle::setWidth(uint16_t width)
{
    Widget::setWidth(width);
    return *this;
}

Rectangle & Rectangle::setHeight(uint16_t height)
{
    Widget::setHeight(height);
    return *this;
}

const Color & Rectangle::color() const
{
    return m_color;
}

Rectangle & Rectangle::setColor(const Color & color)
{
    m_color = color;
    return *this;
}

Rectangle & Rectangle::setColor(uint32_t rgb)
{
    m_color.set(rgb | (m_color.a() << 24));
    return *this;
}

Rectangle & Rectangle::setColor(uint8_t r, uint8_t g, uint8_t b)
{
    m_color.set(r, g, b, m_color.a());
    return *this;
}

const Color & Rectangle::borderColor() const
{
    return m_borderColor;
}

Rectangle & Rectangle::setBorderColor(const Color & borderColor)
{
    m_borderColor = borderColor;
    return *this;
}

Rectangle & Rectangle::setBorderColor(uint32_t rgb)
{
    m_borderColor.set(rgb | (m_borderColor.a() << 24));
    return *this;
}

Rectangle & Rectangle::setBorderColor(uint8_t r, uint8_t g, uint8_t b)
{
    m_borderColor.set(r, g, b, m_borderColor.a());
    return *this;
}

Rectangle & Rectangle::setOpacity(uint8_t opacity)
{
    m_color.setA(opacity);
    m_borderColor.setA(opacity);
    return *this;
}

uint16_t Rectangle::borderWidth() const
{
    return m_borderWidth;
}

Rectangle & Rectangle::setBorderWidth(uint16_t borderWidth)
{
    m_borderWidth = borderWidth;
    return *this;
}

uint16_t Rectangle::radius() const
{
    return m_radius;
}

Rectangle & Rectangle::setRadius(uint16_t radius)
{
    m_radius = radius;
    return *this;
}
}    // namespace FTGUI
