#include "control.h"

namespace FTGUI
{
Button::Button(string label, Widget * parent) :
    Rectangle(parent)
{
    m_name  = "Button";
    m_label = new Label(label, this);
    m_label->setHorizontalAlignment(Label::HCenter);
    m_label->setVerticalAlignment(Label::VCenter);

    m_label->setColor(m_theme->onPrimary());
    if(m_parent->name() == "ButtonGroup")
    {
        setCheckable(true);
    }
    setGeometry(m_x, m_y, m_width, m_height);
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
    Widget::setGeometry(x, y, width, height);
    if(m_checker)
    {
        m_checker->setWidth(m_width - m_width / 5);
        m_checker->setX(m_width / 10);
        m_checker->setHeight(m_height / 12);
        m_checker->setY(m_height - m_checker->height());
    }
}

void Button::setLabel(string text)
{
    m_label->setText(text);
}

bool Button::pressed() const
{
    return m_pressed;
}

void Button::setPressed(bool pressed)
{
    m_pressed = pressed;
    if(pressed)
        setColor(theme()->primaryLight());
    else
        setColor(theme()->primary());
    if(m_visible)
        update();
}

bool Button::touchPressed(int16_t x, int16_t y)
{
    if(!m_enabled)
        return false;
    if(Widget::touchPressed(x, y))
    {
        setPressed(true);
        return true;
    }
    return false;
}

bool Button::touchReleased(int16_t x,
                           int16_t y,
                           int16_t accelerationX,
                           int16_t accelerationY)
{
    if(!m_enabled)
        return false;
    if(Widget::touchReleased(x, y, accelerationX, accelerationY))
    {
        if(!m_pressed)
            return true;
        setPressed(false);
        return true;
    }
    if(!m_pressed)
        return false;
    setPressed(false);
    return false;
}

bool Button::checkable() const
{
    return m_checkable;
}

void Button::setCheckable(bool checkable)
{
    m_checkable = checkable;
    if(m_checkable)
    {
        if(!m_checker)
            m_checker = new Rectangle(this);
        m_checker->setWidth(m_width - m_width / 5);
        m_checker->setX(m_width / 10);
        m_checker->setHeight(m_height / 12);
        m_checker->setColor(Main::LightGrey);
        m_checker->setY(m_height - m_checker->height());
    }
    else
    {
        delete m_checker;
    }
}

bool Button::checked() const
{
    return m_checked;
}

void Button::setChecked(bool checked)
{
    if(!m_checkable)
        return;
    m_checked = checked;
    if(m_checked)
    {
        m_checker->setColor(Main::Error);
    }
    else
    {
        m_checker->setColor(Main::LightGrey);
    }
    if(m_visible)
        update();
}

bool Button::enabled() const
{
    return m_enabled;
}

void Button::setEnabled(bool enabled)
{
    m_enabled = enabled;
    if(m_enabled)
    {
        setColor(theme()->primary());
        m_label->setColor(theme()->onPrimary());
    }
    else
    {
        setColor(Main::LightGrey);
        m_label->setColor(Main::Dark);
    }
    if(m_visible)
        update();
}

void ButtonGroup::addWidget(Widget * widget)
{
    //    if(widget->name() != "Button")
    //    {
    //        error("Button group can contain only button as children \n");
    //        return;
    //    }
    Widget::addWidget(widget);
    m_widgetAdded = true;
}

void ButtonGroup::show()
{
    if(m_widgetAdded)
    {
        for(uint16_t i = 0; i < m_container.size(); ++i)
        {
            auto w = childAt(i);
            w->setCheckable(true);
            if(i == 0)
                w->setChecked(true);
            w->onReleased([&, in = i]() {
                if(m_index == in)
                    return;
                this->setIndex(in);
            });
        }
        m_widgetAdded = false;
        setGeometry(m_x, m_y, m_width, m_height);
    }
    Widget::show();
}

void ButtonGroup::setGeometry(int32_t  x,
                              int32_t  y,
                              uint16_t width,
                              uint16_t height)
{
    Widget::setGeometry(x, y, width, height);
    uint16_t fWidth = (m_width / m_container.size())
                      - /*(m_container.size() * m_padding) -*/ m_padding;

    for(uint16_t i = 0; i < m_container.size(); ++i)
    {
        this->childAt(i)->setGeometry((i * (fWidth + m_padding)) + (m_padding / 2),
                                      0,
                                      fWidth,
                                      m_height);
    }
}

uint16_t ButtonGroup::index() const
{
    return m_index;
}

void ButtonGroup::setIndex(const uint16_t & index)
{
    if(index == m_index)
        return;
    this->childAt(m_index)->setChecked(false);
    m_index = index;
    this->childAt(m_index)->setChecked(true);
    if(m_onIndexChanged)
        m_onIndexChanged->operator()(m_index);
}

uint8_t ButtonGroup::padding() const
{
    return m_padding;
}

void ButtonGroup::setPadding(const uint8_t & padding)
{
    m_padding = padding;
}

void ButtonGroup::setRadius(uint8_t radius)
{
    for(uint16_t i = 0; i < m_container.size(); ++i)
    {
        childAt(i)->setRadius(radius);
    }
}

float RangeController::min() const
{
    return m_min;
}

void RangeController::setMin(float min)
{
    m_min   = min;
    m_range = m_max - m_min;
}

float RangeController::max() const
{
    return m_max;
}

void RangeController::setMax(float max)
{
    m_max   = max;
    m_range = m_max - m_min;
}

float RangeController::value() const
{
    return m_value;
}

void RangeController::setValue(float value)
{
    m_value = value;
    m_onValueChanged->operator()(value);
    //TODO: Move update from here to global level update function
    update();
}

}    // namespace FTGUI
