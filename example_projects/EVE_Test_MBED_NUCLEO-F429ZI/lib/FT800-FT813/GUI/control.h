#ifndef CONTROL_H
#define CONTROL_H

#include <graphics.h>

namespace FTGUI
{
class RangeController
{
public:
    enum Type
    {
        Linear,
        Logarithmic
    };
    RangeController() {}

    float min() const;
    void  setMin(float min);

    float max() const;
    void  setMax(float max);

    float value() const;
    void  setValue(float value);

protected:
    float m_min{0}, m_max{0}, m_value{0};
};

class Button : public Rectangle
{
public:
    Button(string label = "", Widget * parent = nullptr);

    Button(Widget * parent = nullptr) :
        Button("", parent) {}

    void setGeometry(int32_t  x,
                     int32_t  y,
                     uint16_t width  = 80,
                     uint16_t height = 40) override;
    void setLabel(std::string text);

    bool pressed() const;
    void setPressed(bool pressed);

    bool enabled() const;
    void setEnabled(bool enabled);

    bool checkable() const;
    void setCheckable(bool checkable);

    bool checked() const;
    void setChecked(bool checked);

    virtual bool touchPressed(int16_t x, int16_t y) override;
    virtual bool touchReleased(int16_t x,
                               int16_t y,
                               int16_t accelerationX,
                               int16_t accelerationY) override;

protected:
    Label *     m_label{nullptr};
    Rectangle * m_checker{nullptr};
    bool        m_enabled{true},
        m_pressed{false},
        m_checkable{false},
        m_checked{false};
};

class ButtonGroup : public Widget
{
public:
    ButtonGroup(Widget * parent = nullptr) :
        Widget(parent) { m_name = "ButtonGroup"; }

    virtual void addWidget(Widget * widget) override;
    virtual void show() override;

    virtual Button * childAt(uint16_t index) override
    {
        return reinterpret_cast<Button *>(Widget::childAt(index));
    }

    virtual void setGeometry(int32_t  x,
                             int32_t  y,
                             uint16_t width,
                             uint16_t height) override;

    template<typename... Args>
    void onIndexChanged(Args &&... args)
    {
        if(m_onIndexChanged)
            delete m_onIndexChanged;
        m_onIndexChanged = wrapCB<uint16_t>(args...);
    }

    uint16_t index() const;
    void     setIndex(const uint16_t & index);

    uint8_t padding() const;
    void    setPadding(const uint8_t & padding);

protected:
    uint16_t                        m_index{0};
    uint8_t                         m_padding{2};
    bool                            m_widgetAdded{false};
    std::function<void(uint16_t)> * m_onIndexChanged{nullptr};
};
}    // namespace FTGUI

#endif    // CONTROL_H
