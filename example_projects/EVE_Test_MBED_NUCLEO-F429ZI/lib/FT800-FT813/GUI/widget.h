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

#include <colors.h>
#include <ft8xx.h>

namespace FTGUI
{
using namespace EVE;

class Widget
{
public:
    Widget(Widget * parent);

    Widget(int32_t  x,
           int32_t  y,
           uint16_t width,
           uint16_t height,
           Widget * parent);

    virtual void addWidget(Widget * widget);

    template<typename... Tail>
    void addWidgets(Widget * widget, Tail... tail)
    {
        addWidget(widget);
        addWidgets(tail...);
    }

    virtual void removeWidget(Widget * widget);

    virtual ~Widget();

    virtual void show();
    virtual void hide();

    virtual Widget & setGeometry(int32_t  x,
                                 int32_t  y,
                                 uint16_t width,
                                 uint16_t height);

    const string & name() const;

    Widget * parent() const;
    void     setParent(Widget * parent);

    FT8xx *      driver() const;
    Theme *      theme() const;
    EventQueue * queue() const;

    ScreenOrientation orientation() const;

    void setTheme(Theme * theme);

    uint16_t width() const;
    uint16_t height() const;

    int32_t  y() const;
    int32_t  x() const;
    uint16_t z() const;

    virtual Widget & setZ(uint16_t z);
    virtual Widget & setX(int32_t x);
    virtual Widget & setY(int32_t y);
    virtual Widget & setWidth(uint16_t width);
    virtual Widget & setHeight(uint16_t height);

    bool         touchEnable() const;
    virtual void removeCallback();

    template<typename... Args>
    void setCallback(Args... args)
    {
        m_touchTag    = m_driver->setCallbackToTag(args...);
        m_touchEnable = true;
        translateTouchEvent();

        if(m_visible == true)
            update();
    }

    uint8_t touchTag() const;
    void    setTouchTag(const uint8_t & touchTag);

    bool visible() const;
    void setVisible(bool visible);
    bool toggleVisible();

    int32_t absX() const;
    int32_t absY() const;

    //*****animation

    void translateX(int32_t newX)
    {
        animation(&m_x, m_x, newX);
    }

protected:
    enum AnimationOpt : uint32_t
    {
        Duration = 400,
        Delay    = 20
    };
    virtual void update();
    virtual void animationStarted(uint32_t duration = AnimationOpt::Duration,
                                  uint8_t  delay    = AnimationOpt::Delay);

    virtual void translateTouchEvent();
    bool         checkPositionInScreen();

    virtual void animation(int32_t *       value,
                           int32_t         from,
                           int32_t         to,
                           uint32_t        duration = AnimationOpt::Duration,
                           FT8xx::FadeType fadeType = FT8xx::Quad,
                           uint8_t         delay    = AnimationOpt::Delay);

    Widget *     m_parent{nullptr};
    Theme *      m_theme{nullptr};
    FT8xx *      m_driver{nullptr};
    EventQueue * m_queue{nullptr};

    ScreenOrientation     m_orientation{};
    std::vector<Widget *> m_container;
    bool                  m_visible{false};
    bool                  m_touchEnable{false};
    uint8_t               m_touchTag{0};

    string m_name{"Widget"};

    int32_t m_x{0},
        m_y{0};
    uint16_t m_z{0},
        m_width{0},
        m_height{0};
};
}    // namespace FTGUI

#endif    // WIDGET_H
