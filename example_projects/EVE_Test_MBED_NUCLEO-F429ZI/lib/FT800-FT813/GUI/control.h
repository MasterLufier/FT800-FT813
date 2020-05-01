#ifndef CONTROL_H
#define CONTROL_H

#include <graphics.h>

namespace FTGUI
{
class Button : public Widget
{
public:
    Button(string label = "", Widget * parent = nullptr);

    Button(Widget * parent = nullptr) :
        Button("", parent){};

    Widget & setGeometry(uint16_t x,
                         uint16_t y,
                         uint16_t width  = 0,
                         uint16_t height = 0) override;
    void     show() override;
    void     setLabel(std::string text);

    //    Button & setCallback(mbed::Callback<void(uint8_t)> f) override;

protected:
    Rectangle * m_background{nullptr};
    Label *     m_label{nullptr};
};
}    // namespace FTGUI

#endif    // CONTROL_H
