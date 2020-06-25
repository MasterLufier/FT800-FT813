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
#include <type_traits>

namespace FTGUI
{
using namespace EVE;

class Widget
{
    friend class List;
    friend class Page;
    friend class Dialog;

public:
    Widget(Widget * parent, bool modal = false);

    Widget(int32_t  x,
           int32_t  y,
           uint16_t width,
           uint16_t height,
           Widget * parent,
           bool     modal = false);

    virtual void addWidget(Widget * widget);

    template<typename... Tail>
    void addWidgets(Widget * widget, Tail... tail)
    {
        addWidget(widget);
        addWidgets(tail...);
    }

    virtual Widget * childAt(uint16_t index)
    {
        debug_if(index > m_container.size(), "Index out of range");
        return m_container.at(index);
    }

    virtual void removeWidget(Widget * widget);
    virtual ~Widget();

    virtual void show();
    virtual void hide();

    virtual void setGeometry(int32_t  x,
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

    virtual void setZ(uint16_t z);
    virtual void setX(int32_t x);
    virtual void setY(int32_t y);
    virtual void setWidth(uint16_t width);
    virtual void setHeight(uint16_t height);

    //***Touchscreen
    virtual bool touchPressed(int16_t x, int16_t y);
    virtual bool touchChanged(int16_t         x,
                              int16_t         y,
                              const int16_t * accelerationX,
                              const int16_t * accelerationY);
    virtual bool touchReleased(int16_t x,
                               int16_t y,
                               int16_t accelerationX,
                               int16_t accelerationY);

    template<typename... Args>
    void onPressed(Args &&... args)
    {
        if(m_onPressed)
            delete m_onPressed;
        m_onPressed = wrapCB<uint16_t, uint16_t>(args...);
    }

    template<typename... Args>
    void onChanged(Args &&... args)
    {
        if(m_onChanged)
            delete m_onChanged;
        m_onChanged = wrapCB<uint16_t, uint16_t>(args...);
    }

    template<typename... Args>
    void onReleased(Args &&... args)
    {
        if(m_onReleased)
            delete m_onReleased;
        m_onReleased = wrapCB<uint16_t, uint16_t>(args...);
    }

    bool visible() const;
    void setVisible(bool visible);
    bool toggleVisible();

    virtual int32_t absX() const;
    virtual int32_t absY() const;

    //*****animation

    inline void translateX(int16_t newX)
    {
        animation(&m_x, m_x, newX);
    }

    inline void translateY(int16_t newY)
    {
        animation(&m_y, m_y, newY);
    }

    bool modal() const;

protected:
    enum AnimationOpt : uint32_t
    {
        Duration = 400,
        Delay    = 20
    };
    virtual void update();
    virtual void animationStarted(void *   value,
                                  uint32_t duration = AnimationOpt::Duration,
                                  uint8_t  delay    = AnimationOpt::Delay);

    bool checkPositionInScreen();

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
    bool         m_modal{false};

    std::vector<void *> m_animationBlock;

    ScreenOrientation                         m_orientation{};
    std::vector<Widget *>                     m_container;
    bool                                      m_visible{false};
    std::function<void(uint16_t, uint16_t)> * m_onPressed{nullptr};
    std::function<void(uint16_t, uint16_t)> * m_onChanged{nullptr};
    std::function<void(uint16_t, uint16_t)> * m_onReleased{nullptr};

    string m_name{"Widget"};

    int32_t m_x{0},
        m_y{0};
    uint16_t m_z{0},
        m_width{0},
        m_height{0};

    template<typename... ArgsFn,
             typename F,
             typename... Args,
             std::enable_if_t<!std::is_invocable<F, Args...>::value, int> = 0>
    std::function<void(ArgsFn...)> * wrapCB(F && f, Args &&... args)
    {
        return new std::function<void(ArgsFn...)>(
            [f, args...](ArgsFn &&... argsFn) -> void {
                (f)(args..., argsFn...);
            });
    }

    template<typename... ArgsFn,
             typename F,
             typename... Args,
             std::enable_if_t<std::is_invocable<F, Args...>::value, int> = 0>
    std::function<void(ArgsFn...)> * wrapCB(F && f, Args &&... args)
    {
        return new std::function<void(ArgsFn...)>(
            [f, args...](ArgsFn &&...) -> void {
                (f)(args...);
            });
    }

    template<typename... ArgsFn,
             typename R,
             typename T,
             typename U,
             typename... Types,
             typename... Args,
             std::enable_if_t<!(sizeof...(Types)
                                == sizeof...(Args)),
                              int> = 0>
    std::function<void(ArgsFn...)> * wrapCB(U * obj,
                                            R (T::*method)(Types...),
                                            Args &&... args)
    {
        return new std::function<void(ArgsFn...)>(
            [obj, method, args...](ArgsFn &&... argsFn) -> void {
                (obj->*method)(args..., argsFn...);
            });
    }

    template<typename... ArgsFn,
             typename R,
             typename T,
             typename U,
             typename... Types,
             typename... Args,
             std::enable_if_t<(sizeof...(Types)
                               == sizeof...(Args)),
                              int> = 0>
    std::function<void(ArgsFn...)> * wrapCB(U * obj,
                                            R (T::*method)(Types...),
                                            Args &&... args)
    {
        return new std::function<void(ArgsFn...)>(
            [obj, method, args...](ArgsFn &&...) -> void {
                (obj->*method)(args...);
            });
    }

    template<typename FnR,
             typename... ArgsFn,
             typename F,
             typename... Types,
             typename... Args,
             std::enable_if_t<!(sizeof...(Types)
                                == sizeof...(Args)),
                              int> = 0>
    std::function<FnR(ArgsFn...)> * wrapCB(F && (*f)(Types...),
                                           Args &&... args)
    {
        return new std::function<FnR(ArgsFn...)>(
            [f, args...](ArgsFn &&... argsFn) -> FnR {
                (*f)(args..., argsFn...);
            });
    }

    template<typename... ArgsFn,
             typename F,
             typename... Types,
             typename... Args,
             std::enable_if_t<(sizeof...(Types)
                               == sizeof...(Args)),
                              int> = 0>
    std::function<void(ArgsFn...)> * wrapCB(F && (*f)(Types...),
                                            Args &&... args)
    {
        return new std::function<void(ArgsFn...)>(
            [f, args...](ArgsFn &&...) -> void {
                (*f)(args...);
            });
    }
};
}    // namespace FTGUI

#endif    // WIDGET_H
