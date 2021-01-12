#include "modalwidget.h"

namespace FTGUI
{
ModalWidget::ModalWidget(Widget * parent) :
    Widget(parent, true)
{
}

void ModalWidget::prepare()
{
    //Take snapshot
    m_scrim->takeSnapshot();
}

void ModalWidget::open()
{
    this->setVisible(true);
    update();
}

void ModalWidget::close()
{
    this->setVisible(false);
    update();
}

void ModalWidget::destroy()
{
    this->setToDelete(true);
}

Scrim * ModalWidget::scrim() const
{
    return m_scrim;
}

}    // namespace FTGUI
