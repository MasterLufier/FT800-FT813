/*
 * @file pallet.h
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
#ifndef COLORS_H
#define COLORS_H

#include <EVE_config.h>
#include <mbed.h>

namespace FTGUI
{
class Color
{
public:
    constexpr Color() :
        Color(0, 0, 0, 255) {}
    constexpr Color(uint32_t c) :
        m_hex(c | (0xffu << 24)) {}
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) :
        m_hex((a << 24) | (r << 16) | (g << 8) | b) {}
    void set(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
    {
        m_hex = (a << 24) | (r << 16) | (g << 8) | b;
    }
    void set(uint32_t c) { m_hex = c; }
    void setA(uint8_t a)
    {
        m_hex &= ~(0xffUL << 24);
        m_hex |= (uint32_t)a << 24;
    }
    uint32_t hexa() const { return m_hex; }
    uint32_t hex() const { return m_hex & 0x00ffffff; }
    uint8_t  r() const { return (m_hex & 0x000000ff); }
    uint8_t  g() const { return (m_hex & 0x0000ff00) >> 8; }
    uint8_t  b() const { return (m_hex & 0x00ff0000) >> 16; }
    uint8_t  a() const { return (m_hex & 0xff000000) >> 24; }

private:
    uint32_t m_hex;
};

enum ScreenOrientation : uint8_t
{
    Horizontal,
    Vertical
};

// enum Shade
//{
//    _50,
//    _100,
//    _200,
//    _300,
//    _400,
//    _500,
//    _600,
//    _700,
//    _800,
//    _900,
//    A100,
//    A200,
//    A400,
//    A700
//};

/* Default Material Design Color Pallets from
 * https://material.io/design/color/the-color-system.html#tools-for-picking-colors
 */

struct Main
{
    constexpr static Color White{0xffffff}, Black = {0x000000},
                                            Error = {0xB00020}, Dark = {0x2C2C2C};
};

// struct Red
//{
//    static const uint32_t _100 =        0xFFCDD2,
//                          _200 =        0xEF9A9A,
//                          _300 =        0xE57373,
//                          _400 =        0xEF5350,
//                          _500 =        0xF44336,
//                          _600 =        0xE53935,
//                          _700 =        0xD32F2F,
//                          _800 =        0xC62828,
//                          _900 =        0xB71C1C,
//                          A100 =        0xFF8A80,
//                          A200 =        0xFF5252,
//                          A400 =        0xFF1744,
//                          A700 =        0xD50000;
//};

// struct Pink
//{
//    static const uint32_t _50  =        0xFCE4EC,
//                          _100 =        0xF8BBD0,
//                          _200 =        0xF48FB1,
//                          _300 =        0xF06292,
//                          _400 =        0xEC407A,
//                          _500 =        0xE91E63,
//                          _600 =        0xD81B60,
//                          _700 =        0xC2185B,
//                          _800 =        0xAD1457,
//                          _900 =        0x880E4F,
//                          A100 =        0xFF80AB,
//                          A200 =        0xFF4081,
//                          A400 =        0xF50057,
//                          A700 =        0xC51162;
//};

// struct Purple
//{
//    static const uint32_t _50  =        0xF3E5F5,
//                          _100 =        0xE1BEE7,
//                          _200 =        0xCE93D8,
//                          _300 =        0xBA68C8,
//                          _400 =        0xAB47BC,
//                          _500 =        0x9C27B0,
//                          _600 =        0x8E24AA,
//                          _700 =        0x7B1FA2,
//                          _800 =        0x6A1B9A,
//                          _900 =        0x4A148C,
//                          A100 =        0xEA80FC,
//                          A200 =        0xE040FB,
//                          A400 =        0xD500F9,
//                          A700 =        0xAA00FF;
//};

struct Indigo
{
    constexpr static Color _50  = {0xE8EAF6},
                           _100 = {0xC5CAE9},
                           _200 = {0x9FA8DA},
                           _300 = {0x7986CB},
                           _400 = {0x5C6BC0},
                           _500 = {0x3F51B5},
                           _600 = {0x3949AB},
                           _700 = {0x303F9F},
                           _800 = {0x283593},
                           _900 = {0x1A237E},
                           A100 = {0x8C9EFF},
                           A200 = {0x536DFE},
                           A400 = {0x3D5AFE},
                           A700 = {0x304FFE};
};

class Theme : private NonCopyable<Theme>
{
public:
    Theme(string name           = "Default",
          Color  primary        = Indigo::_400,
          Color  primaryLight   = Indigo::_200,
          Color  primaryDark    = Indigo::_800,
          Color  secondary      = Indigo::_400,
          Color  secondaryLight = Indigo::_200,
          Color  secondaryDark  = Indigo::_800,
          Color  background     = Main::White,
          Color  surface        = Main::White,
          Color  error          = Main::Error,
          Color  onPrimary      = Main::White,
          Color  onSecondary    = Main::Black,
          Color  onBackground   = Main::Black,
          Color  onSurface      = Main::Black,
          Color  onError        = Main::White) :
        m_name{name},
        m_primary{primary},
        m_primaryLight{primaryLight},
        m_primaryDark{primaryDark},

        m_secondary{secondary},
        m_secondaryLight{secondaryLight},
        m_secondaryDark{secondaryDark},

        m_background{background},
        m_surface{surface},
        m_error{error},

        m_onPrimary{onPrimary},
        m_onSecondary{onSecondary},
        m_onBackground{onBackground},
        m_onSurface{onSurface},
        m_onError{onError}
    {}

    const Color & background() const;
    const Color & primary() const;
    const Color & primaryLight() const;
    const Color & primaryDark() const;

    const Color & secondary() const;
    const Color & secondaryLight() const;
    const Color & secondaryDark() const;

    const Color & onPrimary() const;

private:
    string m_name{};
    Color  m_primary{},
        m_primaryLight{},
        m_primaryDark{},
        m_secondary{},
        m_secondaryLight{},
        m_secondaryDark{},
        m_background{},
        m_surface{},
        m_error{},
        m_onPrimary{},
        m_onSecondary{},
        m_onBackground{},
        m_onSurface{},
        m_onError{};
};

class Dark : public Theme
{
public:
    Dark() :
        Theme("Dark",
              Indigo::_400,
              Indigo::_200,
              Indigo::_800,
              Indigo::_400,
              Indigo::_200,
              Indigo::_800,
              Main::Black,
              Main::Dark,
              Main::Error,
              Main::White,
              Main::Black,
              Main::White,
              Main::White,
              Main::White)
    {}
};
}    // namespace FTGUI

#endif    // COLORS_H
