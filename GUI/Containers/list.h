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
    void setGeometry(int32_t  x,
                     int32_t  y,
                     uint16_t width,
                     uint16_t height) override;

    Orientation orientation() const;
    void        setOrientation(const Orientation & orientation);

    uint16_t visibleItemCount() const;
    void     setVisibleItemCount(const uint16_t & visibleItemCount);

    bool scrollable() const;
    void setScrollable(bool scrollable);

    bool             touchPressed(int16_t x, int16_t y) override;
    bool             touchChanged(int16_t         x,
                                  int16_t         y,
                                  const int16_t * accelerationX,
                                  const int16_t * accelerationY) override;
    bool             touchReleased(int16_t x,
                                   int16_t y,
                                   int16_t accelerationX,
                                   int16_t accelerationY) override;
    virtual Widget * childAt(uint16_t index) override;

    template<typename... Args>
    void onIndexChanged(Args &&... args)
    {
        if(m_onIndexChanged)
            delete m_onIndexChanged;
        m_onIndexChanged = wrapCB<uint16_t>(args...);
    }
    void     push();
    void     pop();
    void     setIndex(uint16_t index);
    uint16_t index() const;

private:
    Orientation                     m_orientation{Horizontal};
    uint16_t                        m_visibleItemCount{0}, m_itemCount{0}, m_index{0};
    uint16_t                        m_prevPosition{0};
    bool                            m_scrollable{true};
    Widget *                        m_contentItem{nullptr};
    std::function<void(uint16_t)> * m_onIndexChanged{nullptr};
};
}    // namespace FTGUI

#endif    // LIST_H
