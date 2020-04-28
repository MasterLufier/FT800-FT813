/*
 * @file rectangle.h
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
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <widget.h>

namespace FTGUI
{
class Rectangle : public Widget
{
public:
    Rectangle(string name = "Rect", Widget * parent = nullptr) :
        Widget(name, parent) {}
    Rectangle(uint16_t x      = 0,
              uint16_t y      = 0,
              uint16_t width  = 0,
              uint16_t height = 0,
              string   name   = "Rect",
              Widget * parent = nullptr);

    virtual void show() override;
    Rectangle &  setGeometry(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;

    const Color & color() const;
    Rectangle &   setColor(const Color & color);
    Rectangle &   setColor(uint32_t rgb);
    Rectangle &   setColor(uint8_t r,
                           uint8_t g,
                           uint8_t b);

    const Color & borderColor() const;
    Rectangle &   setBorderColor(const Color & borderColor);
    Rectangle &   setBorderColor(uint32_t rgb);
    Rectangle &   setBorderColor(uint8_t r,
                                 uint8_t g,
                                 uint8_t b);

    Rectangle & setOpacity(uint8_t opacity);

    uint16_t    borderWidth() const;
    Rectangle & setBorderWidth(uint16_t borderWidth);

    uint16_t    radius() const;
    Rectangle & setRadius(uint16_t radius);

protected:
    //virtual void render() override;
    Color    m_color{m_theme->primary()};
    Color    m_borderColor{m_theme->onPrimary()};
    uint16_t m_borderWidth{0}, m_radius{0};
};
}    // namespace FTGUI
#endif    // GRAPHICS_H
