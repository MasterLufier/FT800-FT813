#ifndef MODALWIDGET_H
#define MODALWIDGET_H

#include <graphics.h>

namespace FTGUI
{
class ModalWidget : public Widget
{
public:
    ModalWidget(Widget * parent);
    virtual void prepare();
    virtual void open();
    virtual void close();
    virtual void destroy();

    Scrim * scrim() const;

protected:
    Scrim *  m_scrim{nullptr};
    Widget * m_contentItem{nullptr};
};
}    // namespace FTGUI

#endif    // MODALWIDGET_H
