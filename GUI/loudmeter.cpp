#include "loudmeter.h"

namespace FTGUI
{
LoudMeter::LoudMeter(Widget * parent) :
    Widget(parent), RangeController()
{
}

void LoudMeter::show()
{
    Widget::show();
    //    if(m_value)
    m_driver->scissor(absX(), absY(), m_width, m_height / 5);
    m_driver->gradientA(m_x,
                        m_y,
                        Main::Red.hexa(),
                        m_x,
                        m_y + (m_height / 5),
                        Main::Yellow.hexa());
    m_driver->scissor(absX(), absY() + (m_height / 5), m_width, m_height - (m_height / 5));
    m_driver->gradientA(m_x,
                        m_y + (m_height / 5),
                        Main::Yellow.hexa(),
                        m_x,
                        m_height,
                        Main::Green.hexa());
    m_driver->scissor();
}
}    // namespace FTGUI
