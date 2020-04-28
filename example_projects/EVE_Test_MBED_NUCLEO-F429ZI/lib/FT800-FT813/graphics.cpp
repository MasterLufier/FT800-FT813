/*
 * @file rectangle.cpp
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
#include "graphics.h"

namespace FTGUI {
Rectangle::Rectangle(uint16_t x,
                     uint16_t y,
                     uint16_t width,
                     uint16_t height,
                     string name,
                     Widget *parent) :
      Widget(name, parent)
{
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
}
//#define EVE_ZERO                 0UL
//#define EVE_ONE                  1UL
//#define EVE_SRC_ALPHA            2UL
//#define EVE_DST_ALPHA            3UL
//#define EVE_ONE_MINUS_SRC_ALPHA  4UL
//#define EVE_ONE_MINUS_DST_ALPHA  5UL
void Rectangle::show()
{
    //    EVE_cmd_dl(CMD_DLSTART);
//    EVE_cmd_dl( SCISSOR_SIZE(m_width+1,
//                            m_height+1) );
//    EVE_cmd_dl( SCISSOR_XY(m_x-1,
//                          m_y-1) );
//    EVE_cmd_dl(DL_CLEAR_RGB | m_theme->background().hex());
//    EVE_cmd_dl(CLEAR(1, 1, 1));
    EVE_cmd_dl(DL_COLOR_RGB | (m_color.hex()));
    EVE_cmd_dl(COLOR_A(m_color.a()));
    EVE_cmd_dl(BEGIN(EVE_RECTS));
    m_driver->drawVertexPointF(m_x+m_borderWidth,
                               m_y+m_borderWidth);
    m_driver->drawVertexPointF(m_x+m_width-m_borderWidth,
                               m_y+m_height-m_borderWidth);
    EVE_cmd_dl(END());
    //    EVE_cmd_rect(m_x, m_y, m_x+m_width-1, m_y+m_height-1, 5);
//    EVE_cmd_dl(CMD_SWAP);
    //    EVE_cmd_execute();
    //    if(m_borderWidth != 0)
    //    {
    //        //Set Border color
    //        EVE_cmd_dl(DL_COLOR_RGB | (m_borderColor.hex()));
    //        //
    //        if(m_borderColor.a() == 0xff)
    //        {
    //            EVE_cmd_dl(BLEND_FUNC(EVE_ONE, EVE_ZERO));
    //            //set Border line width
    //            EVE_cmd_dl(LINE_WIDTH(1));
    //        }
    //        else
    //        {
    //            EVE_cmd_dl(BLEND_FUNC(EVE_SRC_ALPHA,
    //                                  EVE_ONE));
    //            EVE_cmd_dl(COLOR_A(m_borderColor.a()));
    //            //set Border line width
    //            EVE_cmd_dl(LINE_WIDTH(8));
    //        }
    //        //Draw line if border width 1
    //        if(m_borderWidth == 1)
    //        {
    //            EVE_cmd_dl(BEGIN(EVE_LINES));
    //            m_driver->drawVertexPointF(m_x+m_borderWidth,
    //                                       m_y);
    //            m_driver->drawVertexPointF(m_x+m_width-m_borderWidth,
    //                                       m_y);

    //            m_driver->drawVertexPointF(m_x+m_width,
    //                                       m_y+m_borderWidth);
    //            m_driver->drawVertexPointF(m_x+m_width,
    //                                       m_y+m_height-m_borderWidth);

    //            m_driver->drawVertexPointF(m_x+m_width-m_borderWidth,
    //                                       m_y+m_height);
    //            m_driver->drawVertexPointF(m_x+m_borderWidth,
    //                                       m_y+m_height);

    //            m_driver->drawVertexPointF(m_x,
    //                                       m_y+m_height-m_borderWidth);
    //            m_driver->drawVertexPointF(m_x,
    //                                       m_y+m_borderWidth);
    //            EVE_cmd_dl(END());
    //            //Draw Corners
    //            //TODO: Add rounds
    //            EVE_cmd_dl(BEGIN(EVE_LINES));
    //            m_driver->drawVertexPointF(m_x, m_y);
    //            m_driver->drawVertexPointF(m_x, m_y);
    //            m_driver->drawVertexPointF(m_x+m_width, m_y);
    //            m_driver->drawVertexPointF(m_x+m_width, m_y);
    //            m_driver->drawVertexPointF(m_x+m_width, m_y+m_height);
    //            m_driver->drawVertexPointF(m_x+m_width, m_y+m_height);
    //            m_driver->drawVertexPointF(m_x, m_y+m_height);
    //            m_driver->drawVertexPointF(m_x, m_y+m_height);
    //            EVE_cmd_dl(END());
    //        }
    //        else
    //        {
    //            EVE_cmd_dl(BEGIN(EVE_RECTS));
    //            m_driver->drawVertexPointF(m_x+m_borderWidth,
    //                                       m_y);
    //            m_driver->drawVertexPointF(m_x+m_width-m_borderWidth,
    //                                       m_y+m_borderWidth-1);
    //            EVE_cmd_dl(END());
    //            EVE_cmd_dl(BEGIN(EVE_RECTS));
    //            m_driver->drawVertexPointF(m_x+m_width,
    //                                       m_y+m_borderWidth);
    //            m_driver->drawVertexPointF(m_x+m_width-m_borderWidth+1,
    //                                       m_y+m_height-m_borderWidth);
    //            EVE_cmd_dl(END());
    //            EVE_cmd_dl(BEGIN(EVE_RECTS));
    //            m_driver->drawVertexPointF(m_x+m_width-m_borderWidth,
    //                                       m_y+m_height);
    //            m_driver->drawVertexPointF(m_x+m_borderWidth,
    //                                       m_y+m_height-m_borderWidth+1);
    //            EVE_cmd_dl(END());
    //            EVE_cmd_dl(BEGIN(EVE_RECTS));
    //            m_driver->drawVertexPointF(m_x,
    //                                       m_y+m_height-m_borderWidth);
    //            m_driver->drawVertexPointF(m_x+m_borderWidth-1,
    //                                       m_y+m_borderWidth);
    //            EVE_cmd_dl(END());
    //            //Draw Corners
    //            //TODO: Add rounds
    //            if(m_borderColor.a() != 0xff)
    //            {
    //                EVE_cmd_dl(BLEND_FUNC(EVE_SRC_ALPHA,
    //                                      EVE_ONE_MINUS_SRC_ALPHA));
    //            }
    //            else
    //            {
    //                EVE_cmd_dl(BLEND_FUNC(EVE_ONE,
    //                                      EVE_ONE_MINUS_SRC_ALPHA));
    //            }
    //            EVE_cmd_dl(BEGIN(EVE_RECTS));
    //            m_driver->drawVertexPointF(m_x,
    //                                       m_y);
    //            m_driver->drawVertexPointF(m_x+m_borderWidth-1,
    //                                       m_y+m_borderWidth-1);
    //            EVE_cmd_dl(END());
    //            EVE_cmd_dl(BEGIN(EVE_RECTS));
    //            m_driver->drawVertexPointF(m_x+m_width,
    //                                       m_y);
    //            m_driver->drawVertexPointF(m_x+m_width-m_borderWidth+1,
    //                                       m_y+m_borderWidth-1);
    //            EVE_cmd_dl(END());
    //            EVE_cmd_dl(BEGIN(EVE_RECTS));
    //            m_driver->drawVertexPointF(m_x+m_width,
    //                                       m_y+m_height);
    //            m_driver->drawVertexPointF(m_x+m_width-m_borderWidth+1,
    //                                       m_y+m_height-m_borderWidth+1);
    //            EVE_cmd_dl(END());
    //            EVE_cmd_dl(BEGIN(EVE_RECTS));
    //            m_driver->drawVertexPointF(m_x,
    //                                       m_y+m_height);
    //            m_driver->drawVertexPointF(m_x+m_borderWidth-1,
    //                                       m_y+m_height-m_borderWidth+1);
    //            EVE_cmd_dl(END());
    //        }
    //    }

    //    //Set bg color
    //    EVE_cmd_dl(DL_COLOR_RGB | (m_color.hex()));

    //    if(m_color.a() == 0xff)
    //    {
    //        EVE_cmd_dl(BLEND_FUNC(EVE_ONE, EVE_ZERO));
    //        //set line width
    //        EVE_cmd_dl(LINE_WIDTH(1));
    //    }
    //    else
    //    {
    //        EVE_cmd_dl(BLEND_FUNC(EVE_SRC_ALPHA,
    //                              EVE_ONE));
    //        EVE_cmd_dl(COLOR_A(m_color.a()));
    //        //set Border line width
    //        EVE_cmd_dl(LINE_WIDTH(8));
    //    }
    //    EVE_cmd_dl(BEGIN(EVE_RECTS));
    //    m_driver->drawVertexPointF(m_x+m_borderWidth,
    //                               m_y+m_borderWidth);
    //    m_driver->drawVertexPointF(m_x+m_width-m_borderWidth,
    //                               m_y+m_height-m_borderWidth);
    //    EVE_cmd_dl(END());
    //    //    EVE_cmd_dl(DL_DISPLAY);
    //    //    EVE_cmd_dl(CMD_SWAP);
    //    //    EVE_end_cmd_burst();
    //    //    EVE_cmd_execute();

    Widget::show();
}

Color Rectangle::color() const
{
    return m_color;
}

void Rectangle::setColor(const Color &color)
{
    if(m_borderColor.hex() == m_color.hex())
        m_borderColor = color;
    m_color = color;
}

void Rectangle::setColor(uint32_t abgr)
{
    if(m_borderColor.hex() == m_color.hex())
        m_borderColor.set(abgr);
    m_color.set(abgr);
}

void Rectangle::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if(m_borderColor.hex() == m_color.hex())
        m_borderColor.set(r,g,b,a);
    m_color.set(r,g,b,a);
}

Color Rectangle::borderColor() const
{
    return m_borderColor;
}

void Rectangle::setBorderColor(const Color &borderColor)
{
    m_borderColor = borderColor;
}

void Rectangle::setBorderColor(uint32_t rgba)
{
    m_borderColor.set(rgba);
}

void Rectangle::setBorderColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    m_borderColor.set(r,g,b,a);
}

uint16_t Rectangle::borderWidth() const
{
    return m_borderWidth;
}

void Rectangle::setBorderWidth(const uint16_t &borderWidth)
{
    m_borderWidth = borderWidth;
}
}
