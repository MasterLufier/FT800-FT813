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
    Rectangle(Widget * parent = nullptr);

    Rectangle(uint16_t x      = 0,
              uint16_t y      = 0,
              uint16_t width  = 0,
              uint16_t height = 0,
              Widget * parent = nullptr);

    virtual void        show() override;
    virtual Rectangle & setGeometry(int32_t  x,
                                    int32_t  y,
                                    uint16_t width,
                                    uint16_t height) override;
    virtual Rectangle & setX(int32_t x) override;
    virtual Rectangle & setY(int32_t y) override;
    virtual Rectangle & setZ(uint16_t z) override;
    virtual Rectangle & setWidth(uint16_t width) override;
    virtual Rectangle & setHeight(uint16_t height) override;

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
    Color    m_color{m_theme->primary()};
    Color    m_borderColor{m_theme->onPrimary()};
    uint16_t m_borderWidth{0}, m_radius{0};
};

//**************Fonts
class LFont : private NonCopyable<LFont>
{
public:
    /*! \note Built-in fonts height:
     *
     * Font : pixel Height
     * 16   : 8
     * 17   : 8
     * 18   : 16
     * 19   : 16
     * 20   : 13
     * 21   : 17
     * 22   : 20
     * 23   : 22
     * 24   : 29
     * 25   : 38
     * //
     * 26   : 16
     * 27   : 20
     * 28   : 25
     * 29   : 28
     * 30   : 36
     * 31   : 49
     * 32   : 63
     * 33   : 83
     * 34   : 108
     */
    enum FontType : uint8_t
    {
        Regular,
        Antialiased,
        Monospace
    };

    LFont(uint8_t fontSize, FontType type = Antialiased) :
        m_fontTargetSize(fontSize)
    {
        switch(type)
        {
        case FTGUI::LFont::Regular:
            if(fontSize <= 13)
                m_fontNumber = 20;
            else if(fontSize > 13 && fontSize <= 17)
                m_fontNumber = 21;
            else if(fontSize > 17 && fontSize <= 20)
                m_fontNumber = 22;
            else if(fontSize > 20 && fontSize <= 22)
                m_fontNumber = 23;
            else if(fontSize > 22 && fontSize <= 29)
                m_fontNumber = 24;
            else
                m_fontNumber = 25;
            break;

        case FTGUI::LFont::Antialiased:
            if(fontSize <= 16)
                m_fontNumber = 26;
            else if(fontSize > 16 && fontSize <= 20)
                m_fontNumber = 27;
            else if(fontSize > 20 && fontSize <= 25)
                m_fontNumber = 28;
            else if(fontSize > 25 && fontSize <= 28)
                m_fontNumber = 29;
            else if(fontSize > 28 && fontSize <= 36)
                m_fontNumber = 30;
            else if(fontSize > 36 && fontSize <= 49)
                m_fontNumber = 31;
            else if(fontSize > 49 && fontSize <= 63)
                m_fontNumber = 32;
            else if(fontSize > 63 && fontSize <= 83)
                m_fontNumber = 33;
            else
                m_fontNumber = 34;
            break;

        case FTGUI::LFont::Monospace:
            if(fontSize <= 8)
                m_fontNumber = 16;
            else if(fontSize > 8)
                m_fontNumber = 18;
            break;
        }

        getFontParams();
    }

    uint8_t charWidth(char c) const
    {
        if(c <= 128)
            return width.at(c);
        else
            return 0;
    }
    uint8_t fontHeight() const
    {
        return pixelHeight;
    }

    uint8_t fontNumber() const;

private:
    uint16_t m_fontTargetSize{0};
    FontType m_fontType{Antialiased};
    uint8_t  m_fontNumber{16};
    //Legacy fonts params
    void getFontParams()
    {
        uint32_t fPointer = EVE_memRead32(EVE_ROM_FONT_ADDR);
        uint32_t fontIt   = fPointer + (148 * (m_fontNumber - 16));
        uint16_t i        = 0;
        while(i < 128)
        {
            width.push_back(EVE_memRead8(fontIt + i));
            ++i;
        }
        format      = EVE_memRead32(fontIt + 128);
        lineStride  = EVE_memRead32(fontIt + 132);
        pixelWidth  = EVE_memRead8(fontIt + 136);
        pixelHeight = EVE_memRead8(fontIt + 140);
        gptr        = EVE_memRead32(fontIt + 144);
    }

    uint32_t             format{0};
    uint32_t             lineStride{0};
    uint32_t             gptr{0};
    uint8_t              pixelWidth{0};
    uint8_t              pixelHeight{0};
    std::vector<uint8_t> width{};
};

//**************Label
class Label : public Widget
{
public:
    enum HAlignment : uint16_t
    {
        Left    = 0,
        Right   = EVE_OPT_RIGHTX,
        HCenter = EVE_OPT_CENTERX,
    };
    enum VAlignment : uint16_t
    {
        Top     = 0,
        Bottom  = 1,
        VCenter = EVE_OPT_CENTERY,
    };

    Label(std::string text   = "",
          Widget *    parent = nullptr) :
        Label(text, 0, 0, 0, 0, parent) {}

    void show() override;

    Color color() const;
    void  setColor(const Color & color);

    std::string text() const;
    void        setText(const std::string & label);

    bool fillWidth() const;
    void setFillWidth(bool fillWidth);

    HAlignment horizontalAlignment() const;
    void       setHorizontalAlignment(const HAlignment & horizontalAlignment);

    VAlignment verticalAlignment() const;
    void       setVerticalAlignment(const VAlignment & verticalAlignment);

private:
    Label(std::string label  = "",
          int32_t     x      = 0,
          int32_t     y      = 0,
          uint16_t    width  = 0,
          uint16_t    height = 0,
          Widget *    parent = nullptr);

    bool        m_fillWidth{false};
    VAlignment  m_verticalAlignment{Top};
    HAlignment  m_horizontalAlignment{Left};
    std::string m_text{};
    Color       m_color{m_theme->onPrimary()};
    LFont       m_font{20};
};
}    // namespace FTGUI
#endif    // GRAPHICS_H
