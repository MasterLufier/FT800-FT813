#ifndef DIALOG_H
#define DIALOG_H

#include <graphics.h>
#include <modalwidget.h>
#include <page.h>

namespace FTGUI
{
class Dialog : public ModalWidget
{
public:
    Dialog(Widget * parent);

    void         addWidget(Widget * widget) override;
    virtual void setGeometry(int32_t  x,
                             int32_t  y,
                             uint16_t width,
                             uint16_t height) override;

    void setWidth(uint16_t width) override;
    void setHeight(uint16_t height) override;

protected:
    virtual void setX(int32_t x) override { Widget::setX(x); };
    virtual void setY(int32_t y) override { Widget::setY(y); };
    virtual void setZ(uint16_t z) override { Widget::setZ(z); };
};
}    // namespace FTGUI

#endif    // DIALOG_H
