#include "dialog.h"

namespace FTGUI
{
Dialog::Dialog(Widget * parent) :
    ModalWidget(parent)
{
    m_name   = "Dialog";
    m_x      = 0;
    m_y      = 0;
    m_width  = EVE_HSIZE;
    m_height = EVE_VSIZE;

    m_scrim       = new Scrim(this);
    m_contentItem = new Page(this);
    setGeometry(30, 20, m_width - 60, m_height - 40);
    setZ(99);
}

void Dialog::addWidget(Widget * widget)
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
        m_contentItem = reinterpret_cast<Page *>(widget);
        widget->setParent(this);
        m_container.push_back(widget);
        return;
    }
    m_contentItem->addWidget(widget);
}

void Dialog::setGeometry(int32_t  x,
                         int32_t  y,
                         uint16_t width,
                         uint16_t height)
{
    m_contentItem->setGeometry(x, y, width, height);
}

void Dialog::setWidth(uint16_t width)
{
    m_contentItem->setX((m_width - width) / 2);
    m_contentItem->setWidth(width);
}

void Dialog::setHeight(uint16_t height)
{
    m_contentItem->setY((m_height - height) / 2);
    m_contentItem->setHeight(height);
}
}    // namespace FTGUI
