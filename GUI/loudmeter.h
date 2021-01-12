#ifndef LOUDMETER_H
#define LOUDMETER_H

#include <control.h>
#include <graphics.h>

namespace FTGUI
{
class LoudMeter : public RangeController
{
public:
    LoudMeter(float min = 0, float max = 0, Widget * parent = nullptr);
    void show() override;

private:
};

class ClipMeter : public RangeController
{
public:
    ClipMeter(float min = 0, float max = 0, Widget * parent = nullptr);
    void setGeometry(int32_t  x,
                     int32_t  y,
                     uint16_t width,
                     uint16_t height) override;

private:
    Rectangle m_rect{this};
};
}    // namespace FTGUI

#endif    // LOUDMETER_H
