#ifndef DRAWER_H
#define DRAWER_H

#include <list.h>
#include <modalwidget.h>

namespace FTGUI
{
class Drawer : public ModalWidget
{
public:
    Drawer(Widget * parent);
    virtual void addWidget(Widget * widget) override;

    virtual void setWidth(uint16_t width) override;
    virtual void open() override;
    virtual void close() override;

protected:
    virtual void setHeight(uint16_t) override{};
    virtual void setX(int32_t x) override { Widget::setX(x); };
    virtual void setY(int32_t y) override { Widget::setY(y); };
    virtual void setZ(uint16_t z) override { Widget::setZ(z); };
};
}    // namespace FTGUI
#endif    // DRAWER_H
