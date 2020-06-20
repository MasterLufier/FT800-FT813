#include "drawer.h"

namespace FTGUI
{
Drawer::Drawer(Widget * parent) :
    ModalWidget(parent)
{
    m_name   = "Drawer";
    m_x      = 0;
    m_y      = 0;
    m_width  = EVE_HSIZE;
    m_height = EVE_VSIZE;

    m_scrim          = new Scrim(this);
    auto contentItem = new List(this);
    m_contentItem    = contentItem;
    contentItem->setScrollable(false);
    m_scrim->onReleased(this, &Drawer::close);
    m_contentItem->setGeometry(-200, 0, 200, m_height);
    setZ(99);
}

void Drawer::addWidget(Widget * widget)
{
    if(!m_scrim)
    {
        m_scrim = reinterpret_cast<Scrim *>(widget);
        widget->setParent(this);
        m_container.push_back(widget);
        return;
    }
    if(!m_contentItem)
    {
        m_contentItem = reinterpret_cast<List *>(widget);
        widget->setParent(this);
        m_container.push_back(widget);
        return;
    }
    m_contentItem->addWidget(widget);
}

void Drawer::setWidth(uint16_t width)
{
    m_contentItem->setWidth(width);
}

void Drawer::open()
{
    ModalWidget::open();
    m_contentItem->translateX(0);
}

void Drawer::close()
{
    m_contentItem->translateX(-m_contentItem->width());
    queue()->call_in(AnimationOpt::Duration, [&]() {
        this->setVisible(false);
        this->update();
    });
}
}    // namespace FTGUI
