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
Rectangle::Rectangle(Widget * parent) :
    Rectangle(0, 0, 0, 0, parent) {}

Rectangle::Rectangle(uint16_t x,
                     uint16_t y,
                     uint16_t width,
                     uint16_t height,
                     Widget * parent) :
    Widget(x,
           y,
           width,
           height,
           parent)
{
    m_name = "Rectangle";
}

void Rectangle::show()
{
    if(checkPositionInScreen() == false)
        return;
    //Draw shadow
    if(m_z != 0)
    {
        m_driver->colorRGB(0x040404);
        m_driver->push(blendFunc(AlphaBlending::SrcAlpha,
                                 AlphaBlending::OneMinusSrcAlpha));
        m_driver->lineWidth(24 + ((m_radius - 1) * 16));
        m_driver->begin(Rects);
        float shadow = (m_color.a() < 48 ? m_color.a() : 48);
        for(uint8_t i = m_z; i > 0; --i)
        {
            m_driver->colorA(static_cast<uint8_t>(shadow / i));
            //Draw ambient light shadow
            m_driver->vertexPointF(absX() + m_radius - i,
                                   absY() + m_radius - i);
            m_driver->vertexPointF(absX() + m_width - m_radius + i - 1,
                                   absY() + m_height - m_radius + i - 1);
            //Draw key light shadow
            m_driver->vertexPointF(absX() + m_radius,
                                   absY() + m_radius);
            m_driver->vertexPointF(absX() + m_width - m_radius - 1,
                                   absY() + m_height - m_radius + i - 1);
        }
        //        EVE_cmd_dl(END());
        //        EVE_cmd_dl(BEGIN(EVE_RECTS));
    }
    else
    {
        m_driver->begin(Rects);
    }
    //Draw border if provided
    if(m_borderWidth != 0)
    {
        m_driver->colorRGB(m_borderColor.hex());
        //Fix subpixel line opacity
        if(m_radius == 0)
        {
            if(m_borderColor.a() == 255)
            {
                m_driver->push(BLEND_FUNC(EVE_ONE, EVE_ZERO));
                m_driver->lineWidth(1);
            }
            else
            {
                m_driver->push(BLEND_FUNC(EVE_SRC_ALPHA, EVE_ONE_MINUS_SRC_ALPHA));
                m_driver->colorA(m_borderColor.a());
                m_driver->lineWidth(8);
            }
            m_driver->vertexPointF(absX(),
                                   absY());
            m_driver->vertexPointF(absX() + m_width - 1,
                                   absY() + m_height - 1);
        }
        else
        {
            m_driver->colorA(m_borderColor.a());
            m_driver->push(BLEND_FUNC(EVE_SRC_ALPHA, EVE_ONE_MINUS_SRC_ALPHA));
            m_driver->lineWidth(24 + ((m_radius - 1) * 16));
            m_driver->vertexPointF(absX() + m_radius,
                                   absY() + m_radius);
            m_driver->vertexPointF(absX() + m_width - m_radius - 1,
                                   absY() + m_height - m_radius - 1);
        }
    }
    //Draw body
    m_driver->colorRGB(m_color.hex());
    //Fix subpixel line opacity
    if(m_radius == 0)
    {
        if(m_color.a() == 255)
        {
            m_driver->push(BLEND_FUNC(EVE_ONE, EVE_ZERO));
            m_driver->lineWidth(1);
        }
        else
        {
            m_driver->colorA(m_color.a());
            m_driver->push(BLEND_FUNC(EVE_SRC_ALPHA, EVE_ONE_MINUS_SRC_ALPHA));
            m_driver->lineWidth(8);
        }
    }
    else
    {
        m_driver->colorA(m_borderColor.a());
        m_driver->push(BLEND_FUNC(EVE_SRC_ALPHA, EVE_ONE_MINUS_SRC_ALPHA));
        m_driver->lineWidth(24 + ((m_radius - 1) * 16));
    }

    m_driver->vertexPointF(absX() + m_borderWidth + m_radius,
                           absY() + m_borderWidth + m_radius);

    m_driver->vertexPointF(absX() + m_width - m_borderWidth - m_radius - 1,
                           absY() + m_height - m_borderWidth - m_radius - 1);

    m_driver->end();

    Widget::show();
}

const Color & Rectangle::color() const
{
    return m_color;
}

Rectangle & Rectangle::setColor(const Color & color)
{
    m_color = color;
    if(m_visible != false)
        update();
    return *this;
}

Rectangle & Rectangle::setColor(uint32_t rgb)
{
    m_color.set(rgb | (m_color.a() << 24));
    if(m_visible != false)
        update();
    return *this;
}

Rectangle & Rectangle::setColor(uint8_t r, uint8_t g, uint8_t b)
{
    m_color.set(r, g, b, m_color.a());
    if(m_visible != false)
        update();
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
    if(m_visible != false)
        update();
    return *this;
}

Rectangle & Rectangle::setBorderColor(uint8_t r, uint8_t g, uint8_t b)
{
    m_borderColor.set(r, g, b, m_borderColor.a());
    if(m_visible != false)
        update();
    return *this;
}

Rectangle & Rectangle::setOpacity(uint8_t opacity)
{
    m_color.setA(opacity);
    m_borderColor.setA(opacity);
    if(m_visible != false)
        update();
    return *this;
}

uint16_t Rectangle::borderWidth() const
{
    return m_borderWidth;
}

Rectangle & Rectangle::setBorderWidth(uint16_t borderWidth)
{
    m_borderWidth = borderWidth;
    if(m_visible != false)
        update();
    return *this;
}

uint16_t Rectangle::radius() const
{
    return m_radius;
}

Rectangle & Rectangle::setRadius(uint16_t radius)
{
    m_radius = radius;
    if(m_visible != false)
        update();
    return *this;
}

void Label::show()
{
    if(checkPositionInScreen() == false)
        return;
    m_driver->colorARGB(m_color.hexa());
    m_driver->push(BLEND_FUNC(EVE_SRC_ALPHA, EVE_ONE_MINUS_SRC_ALPHA));
    //TODO: Add font scaling to target size
    //    EVE_cmd_dl(CMD_LOADIDENTITY);
    //    EVE_cmd_scale(65536 / 2, 65536 / 2);
    //    EVE_cmd_dl(CMD_SETMATRIX);
    if(m_fillWidth == true)
    {
        m_driver->push(CMD_FILLWIDTH);
        m_driver->push(m_width);
        if(m_verticalAlignment == Bottom)
        {
            m_driver->text(absX(),
                           absY() - m_font.fontHeight(),
                           m_font.fontNumber(),
                           m_text.c_str(),
                           TextOpt::Fill);
        }
        else
        {
            m_driver->text(absX(),
                           absY(),
                           m_font.fontNumber(),
                           m_text.c_str(),
                           TextOpt::Fill /*static_cast<TextOpt>(m_verticalAlignment | m_horizontalAlignment)*/);
        }
    }
    else
    {
        if(m_verticalAlignment == Bottom)
        {
            m_driver->text(absX(),
                           absY() - m_font.fontHeight(),
                           m_font.fontNumber(),
                           m_text.c_str(),
                           static_cast<TextOpt>(Top | m_horizontalAlignment));
        }
        else
        {
            m_driver->text(absX(),
                           absY(),
                           m_font.fontNumber(),
                           m_text.c_str(),
                           static_cast<TextOpt>(m_verticalAlignment | m_horizontalAlignment));
        }
    }
    Widget::show();
}

Label::Label(string   text,
             int32_t  x,
             int32_t  y,
             uint16_t width,
             uint16_t height,
             Widget * parent) :
    Widget(parent),
    m_text(text)
{
    m_name = "Label";
    m_x    = x;
    m_y    = y;
    if(width == 0 && text.length() != 0)
    {
        for(const auto & c : text)
        {
            m_width += m_font.charWidth(c);
        }
    }
    else
    {
        m_width = width;
    }

    if(height == 0)
    {
        m_height = m_font.fontHeight();
    }
    else
    {
        m_height = height;
    }
}

Label::VAlignment Label::verticalAlignment() const
{
    return m_verticalAlignment;
}

void Label::setVerticalAlignment(const VAlignment & verticalAlignment)
{
    m_verticalAlignment = verticalAlignment;
}

Label::HAlignment Label::horizontalAlignment() const
{
    return m_horizontalAlignment;
}

void Label::setHorizontalAlignment(const HAlignment & horizontalAlignment)
{
    m_horizontalAlignment = horizontalAlignment;
}

bool Label::fillWidth() const
{
    return m_fillWidth;
}

void Label::setFillWidth(bool fillWidth)
{
    m_fillWidth = fillWidth;
}

std::string Label::text() const
{
    return m_text;
}

void Label::setText(const std::string & label)
{
    m_text = label;
}

Color Label::color() const
{
    return m_color;
}

void Label::setColor(const Color & color)
{
    m_color = color;
}

uint8_t LFont::fontNumber() const
{
    return m_fontNumber;
}

Scrim::Scrim(Widget * parent) :
    Rectangle(parent)
{
    m_name     = "Scrim";
    m_snapshot = m_driver->ramG()->saveSnapshot(m_name);
    setGeometry(0, 0, EVE_HSIZE, EVE_VSIZE);
    setColor(Main::Black);
    setOpacity(230);
}

Scrim::~Scrim()
{
    m_driver->ramG()->removeSnapshot(m_snapshot);
    delete m_snapshot;
}

void Scrim::show()
{
    m_driver->push(blendFunc(AlphaBlending::One,
                             AlphaBlending::One));
    m_driver->append(m_snapshot, 0, 0);
    m_driver->push(blendFunc(AlphaBlending::SrcAlpha,
                             AlphaBlending::OneMinusSrcAlpha));
    m_driver->colorA(120);
    m_driver->append(m_snapshot, 0, -1);
    m_driver->append(m_snapshot, -1, 0);
    m_driver->append(m_snapshot, 0, 1);
    m_driver->append(m_snapshot, 1, 0);
    m_driver->colorA(30);
    m_driver->append(m_snapshot, -1, -1);
    m_driver->append(m_snapshot, -1, -1);
    m_driver->append(m_snapshot, 1, 1);
    m_driver->append(m_snapshot, 1, 1);
    Rectangle::show();
}

void Scrim::takeSnapshot()
{
    m_driver->ramG()->updateSnapshot(m_snapshot);
}

}    // namespace FTGUI
