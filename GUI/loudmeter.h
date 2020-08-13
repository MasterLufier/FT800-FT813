#ifndef LOUDMETER_H
#define LOUDMETER_H

#include <control.h>
#include <graphics.h>

namespace FTGUI
{
class LoudMeter : public Widget, public RangeController
{
public:
    LoudMeter(Widget * parent = nullptr);
    void show() override;

private:
};
}    // namespace FTGUI

#endif    // LOUDMETER_H
