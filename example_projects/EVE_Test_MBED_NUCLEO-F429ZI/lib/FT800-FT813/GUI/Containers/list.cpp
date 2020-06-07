#include "list.h"

namespace FTGUI
{
List::List(Widget * parent) :
    Widget(parent)
{
    m_name = "List";
}

void List::addWidget(Widget * widget)
{
    Widget::addWidget(widget);
    uint16_t itemCount = m_visibleItemCount == 0
                                 || m_visibleItemCount > m_container.size()
                             ? m_container.size()
                             : m_visibleItemCount;
    uint16_t i = 0;
    switch(m_orientation)
    {
    case FTGUI::List::Vertical:
    {
        uint16_t width = (this->width() / itemCount) - 1;

        for(const auto & w : m_container)
        {
            w->setGeometry(i * width + 1,
                           0,
                           width,
                           m_height);
            ++i;
        }
        break;
    }
    case FTGUI::List::Horizontal:
    {
        uint16_t height = (this->height() / itemCount) - 1;
        for(const auto & w : m_container)
        {
            w->setGeometry(0,
                           i * height + 1,
                           m_width,
                           height);
            ++i;
        }
        break;
    }
    }
}

void List::show()
{
    Widget::show();
}

List::Orientation List::orientation() const
{
    return m_orientation;
}

void List::setOrientation(const Orientation & orientation)
{
    m_orientation = orientation;
}

uint16_t List::visibleItemCount() const
{
    return m_visibleItemCount;
}

void List::setVisibleItemCount(const uint16_t & visibleItemCount)
{
    m_visibleItemCount = visibleItemCount;
}

bool List::touchPressed(int16_t x, int16_t y)
{
    if(Widget::touchPressed(x, y))
    {
        switch(m_orientation)
        {
        case FTGUI::List::Vertical:
            m_prevPosition = x;
            break;
        case FTGUI::List::Horizontal:
            m_prevPosition = y;
            break;
        }
        return true;
    }
    return false;
}

bool List::touchChanged(int16_t x, int16_t y)
{
    if(Widget::touchChanged(x, y))
    {
        switch(m_orientation)
        {
        case FTGUI::List::Vertical:
        {
            int8_t diff = x - m_prevPosition;
            for(const auto w : m_container)
            {
                w->setX(w->x() + diff);
            }
            this->update();
            m_prevPosition = x;
            break;
        }
        case FTGUI::List::Horizontal:
        {
            int8_t diff = y - m_prevPosition;
            for(const auto w : m_container)
            {
                w->setY(w->y() + diff);
            }
            this->update();
            m_prevPosition = y;
            break;
        }
        }
        return true;
    }
    return false;
}

}    // namespace FTGUI
