#include "control.h"

namespace FTGUI
{
Button::Button(string label, Widget * parent) :
    Widget(parent)
{
    m_background = new Rectangle(this);
    m_label      = new Label(label, this);
}

Widget & Button::setGeometry(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    Widget::setGeometry(x, y, width, height);

    m_background->setColor(m_theme->primary());
    m_label->setColor(m_theme->onPrimary());
    m_label->setGeometry(Widget::x() + m_width / 2,
                         Widget::y() + m_height / 2,
                         Widget::width(),
                         Widget::height());
    m_background->setGeometry(Widget::x(),
                              Widget::y(),
                              Widget::width(),
                              Widget::height());
}

void Button::show()
{
    Widget::show();
}

void Button::setLabel(string text)
{
    m_label->setLabel(text);
}

//Button & Button::setCallback(mbed::Callback<void(uint8_t)> f)
//{
//    Widget::setCallback(f);
//    return *this;
//}

}    // namespace FTGUI
