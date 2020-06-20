#include "control.h"

namespace FTGUI
{
Button::Button(string label, Widget * parent) :
    Widget(parent)
{
    m_name       = "Button";
    m_background = new Rectangle(this);
    m_label      = new Label(label, this);
    m_label->setHorizontalAlignment(Label::HCenter);
    m_label->setVerticalAlignment(Label::VCenter);

    m_background->setColor(m_theme->primary());
    m_label->setColor(m_theme->onPrimary());
}

void Button::setGeometry(int32_t  x,
                         int32_t  y,
                         uint16_t width,
                         uint16_t height)
{
    m_label->setGeometry(width / 2 - 1,
                         height / 2 - 1,
                         width,
                         height);
    m_background->setGeometry(0,
                              0,
                              width,
                              height);
    Widget::setGeometry(x, y, width, height);
}

void Button::show()
{
    Widget::show();
}

void Button::setLabel(string text)
{
    m_label->setText(text);
}

}    // namespace FTGUI
