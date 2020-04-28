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


namespace FTGUI {
//class GraphicalObject : public Widget
//{
//public:
//    GraphicalObject(uint16_t x = 0,
//                    uint16_t y = 0,
//                    uint16_t width = 0,
//                    uint16_t height = 0,
//                    Widget * parent = nullptr) :
//          Widget(parent)
//    {
//         m_x = x;
//         m_y = y;
//         m_width = width;
//         m_height = height;
//    }
//};

class Rectangle : public Widget
{
public:
    Rectangle(string name = "Rect", Widget * parent = nullptr) : Widget(name, parent){}
    Rectangle(uint16_t x = 0,
              uint16_t y = 0,
              uint16_t width = 0,
              uint16_t height = 0,
              string name = "Rect",
              Widget * parent = nullptr);

    virtual void show() override;
    Color color() const;
    void setColor(const Color &color);
    void setColor(uint32_t rgba);
    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    Color borderColor() const;
    void setBorderColor(const Color &borderColor);
    void setBorderColor(uint32_t rgba);
    void setBorderColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    uint16_t borderWidth() const;
    void setBorderWidth(const uint16_t &borderWidth);

protected:
    //    virtual void render() override;
    Color m_color{m_theme->primary()};
    Color m_borderColor{m_color};
    uint16_t m_borderWidth{10}, m_radius{0};
};
}
#endif // GRAPHICS_H
