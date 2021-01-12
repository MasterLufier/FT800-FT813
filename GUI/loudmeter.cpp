#include "loudmeter.h"

namespace FTGUI
{
LoudMeter::LoudMeter(float min, float max, Widget * parent) :
    RangeController(min, max, parent)
{
}

void LoudMeter::show()
{
    RangeController::show();
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

ClipMeter::ClipMeter(float min, float max, Widget * parent) :
    RangeController(min, max, parent)
{
    m_rect.setBorderWidth(1);
    m_rect.setBorderColor(Main::Dark);
    onValueChanged([this]() {
        if(m_value == m_max)
        {
            m_rect.setColor(255, 0, 0);
        }
        else
        {
            //new_value = ( (old_value - old_min) / (old_max - old_min) ) * (new_max - new_min) + new_min
            //            m_rect.setOpacity(255);
            uint16_t value = ((m_value - m_min) / m_range) * 765;
            uint8_t  r{0}, g{0};
            if(value <= 255)
            {
                g = value;
            }
            else if(value > 255 && value <= 512)
            {
                g = 255;
                r = value - 255;
            }
            else
            {
                g = (765 - value);
                r = 255;
            }
            //            r = static_cast<uint8_t>(value - 255);
            //            g = static_cast<uint8_t>(value);
            m_rect.setColor(r, g, 0);
        }
    });
}

void ClipMeter::setGeometry(int32_t x, int32_t y, uint16_t width, uint16_t height)
{
    RangeController::setGeometry(x, y, width, height);
    m_rect.setGeometry(x, y, width, height);
}

}    // namespace FTGUI
