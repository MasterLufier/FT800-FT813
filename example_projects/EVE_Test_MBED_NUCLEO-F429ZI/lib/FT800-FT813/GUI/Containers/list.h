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

    void   addWidget(Widget * widget) override;
    List & setGeometry(int32_t  x,
                       int32_t  y,
                       uint16_t width,
                       uint16_t height) override;

    Orientation orientation() const;
    void        setOrientation(const Orientation & orientation);

    uint16_t visibleItemCount() const;
    void     setVisibleItemCount(const uint16_t & visibleItemCount);

    bool scrollable() const;
    void setScrollable(bool scrollable);

    bool touchPressed(int16_t x, int16_t y) override;
    bool touchChanged(int16_t         x,
                      int16_t         y,
                      const int16_t * accelerationX,
                      const int16_t * accelerationY) override;
    bool touchReleased(int16_t x,
                       int16_t y,
                       int16_t accelerationX,
                       int16_t accelerationY) override;
    void push();
    void pop();

private:
    Orientation m_orientation{Horizontal};
    uint16_t    m_visibleItemCount{0}, m_itemCount{0};
    uint16_t    m_prevPosition{0};
    bool        m_scrollable{true};
    Widget *    m_contentItem{nullptr};
};
}    // namespace FTGUI

#endif    // LIST_H
