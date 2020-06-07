#ifndef LIST_H
#define LIST_H

#include <widget.h>

namespace FTGUI
{
class List : public Widget
{
public:
    enum Orientation
    {
        Vertical,
        Horizontal
    };
    List(Widget * parent);

    void addWidget(Widget * widget) override;
    void show() override;

    Orientation orientation() const;
    void        setOrientation(const Orientation & orientation);

    uint16_t visibleItemCount() const;
    void     setVisibleItemCount(const uint16_t & visibleItemCount);

    bool touchPressed(int16_t x, int16_t y) override;
    bool touchChanged(int16_t x, int16_t y) override;

private:
    Orientation m_orientation{Horizontal};
    uint16_t    m_visibleItemCount{0};
    uint16_t    m_prevPosition{0};
};
}    // namespace FTGUI

#endif    // LIST_H
