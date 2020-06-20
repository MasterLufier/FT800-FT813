#include "list.h"

namespace FTGUI
{
List::List(Widget * parent) :
    Widget(parent)
{
    m_name        = "List";
    m_contentItem = new Widget(this);
}

void List::addWidget(Widget * widget)
{
    if(!m_contentItem)
    {
        m_contentItem = widget;
        widget->setParent(this);
        m_container.push_back(widget);
        return;
    }
    m_contentItem->addWidget(widget);
    m_itemCount = m_visibleItemCount == 0
                          || m_visibleItemCount > m_contentItem->m_container.size()
                      ? m_contentItem->m_container.size()
                      : m_visibleItemCount;
    setGeometry(m_x, m_y, m_width, m_height);
}

void List::setGeometry(int32_t  x,
                       int32_t  y,
                       uint16_t width,
                       uint16_t height)
{
    Widget::setGeometry(x, y, width, height);
    uint16_t i = 0;
    switch(m_orientation)
    {
    case FTGUI::List::Vertical:
    {
        uint16_t fWidth = (m_width / m_itemCount) - 1;
        m_contentItem->setHeight(height);
        for(const auto w : m_contentItem->m_container)
        {
            w->setGeometry(i * fWidth + 1,
                           0,
                           fWidth,
                           m_height);
            ++i;
        }
        if(m_contentItem->m_container.size() != 0)
            m_contentItem->setWidth(m_contentItem->m_container.front()->width()
                                    * m_contentItem->m_container.size());
        break;
    }
    case FTGUI::List::Horizontal:
    {
        uint16_t fHeight = (m_height / m_itemCount) - 1;
        m_contentItem->setWidth(width);
        for(const auto w : m_contentItem->m_container)
        {
            w->setGeometry(0,
                           i * fHeight + 1,
                           m_width,
                           fHeight);
            ++i;
        }
        if(m_contentItem->m_container.size() != 0)
            m_contentItem->setHeight(m_contentItem->m_container.front()->height()
                                     * m_contentItem->m_container.size());
        break;
    }
    }
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
    m_itemCount        = m_visibleItemCount == 0
                          || m_visibleItemCount > m_contentItem->m_container.size()
                      ? m_contentItem->m_container.size()
                      : m_visibleItemCount;
}

bool List::touchPressed(int16_t x, int16_t y)
{
    if(Widget::touchPressed(x, y))
    {
        if(!m_scrollable)
            return true;
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

bool List::touchChanged(int16_t         x,
                        int16_t         y,
                        const int16_t * accelerationX,
                        const int16_t * accelerationY)
{
    if(Widget::touchChanged(x, y, accelerationX, accelerationY))
    {
        if(!m_scrollable)
            return true;
        switch(m_orientation)
        {
        case FTGUI::List::Vertical:
        {
            int8_t diff = x - m_prevPosition;
            if(diff == 0)
                return true;
            m_contentItem->setX(m_contentItem->x() + diff);
            this->update();
            m_prevPosition = x;
            break;
        }
        case FTGUI::List::Horizontal:
        {
            int8_t diff = y - m_prevPosition;
            if(diff == 0)
                return true;
            m_contentItem->setY(m_contentItem->y() + diff);
            this->update();
            m_prevPosition = y;
            break;
        }
        }

        return true;
    }
    return false;
}

bool List::touchReleased(int16_t x,
                         int16_t y,
                         int16_t accelerationX,
                         int16_t accelerationY)
{
    if(Widget::touchReleased(x, y, accelerationX, accelerationY))
    {
        if(!m_scrollable)
            return true;
        switch(m_orientation)
        {
        case FTGUI::List::Vertical:
            if(m_contentItem->x() > 0)
            {
                m_contentItem->translateX(0);
            }
            else if(m_contentItem->x() + m_contentItem->width() < m_width)
            {
                m_contentItem->translateX(-m_contentItem->width() + m_width - 1);
            }
            else if(m_contentItem->width() % m_contentItem->x() != 0)
            {
                int16_t diff = abs(m_contentItem->x() % m_contentItem->m_container.front()->width());
                if(diff == 0)
                    return true;
                if(diff < (m_contentItem->m_container.front()->width() / 2))
                {
                    if(accelerationX > -10)
                    {
                        m_contentItem->translateX(m_contentItem->x() + diff);
                        m_index--;
                    }
                    else
                    {
                        m_contentItem->translateX(
                            m_contentItem->x()
                            - (m_contentItem->m_container.front()->width()
                               - diff));
                        m_index++;
                    }
                }
                else
                {
                    if(accelerationX < 10)
                    {
                        m_contentItem->translateX(
                            m_contentItem->x()
                            - (m_contentItem->m_container.front()->width()
                               - diff));
                        m_index++;
                    }
                    else
                    {
                        m_contentItem->translateX(m_contentItem->x() + diff);
                        m_index--;
                    }
                }
            }
            break;
        case FTGUI::List::Horizontal:
            if(m_contentItem->y() > 0)
            {
                m_contentItem->translateY(0);
            }
            else if(m_contentItem->y() + m_contentItem->height() < m_height)
            {
                m_contentItem->translateY(-m_contentItem->height() + m_height - 1);
            }
            else if(m_contentItem->height() % m_contentItem->y() != 0)
            {
                int16_t diff = abs(m_contentItem->y() % m_contentItem->m_container.front()->height());
                if(diff == 0)
                    return true;
                if(diff < (m_contentItem->m_container.front()->height() / 2))
                {
                    if(accelerationY > -10)
                    {
                        m_contentItem->translateY(m_contentItem->y() + diff);
                        m_index--;
                    }
                    else
                    {
                        m_contentItem->translateY(
                            m_contentItem->y()
                            - (m_contentItem->m_container.front()->height()
                               - diff));
                        m_index++;
                    }
                }
                else
                {
                    if(accelerationY < 10)
                    {
                        m_contentItem->translateY(
                            m_contentItem->y()
                            - (m_contentItem->m_container.front()->height()
                               - diff));
                        m_index++;
                    }
                    else
                    {
                        m_contentItem->translateY(m_contentItem->y() + diff);
                        m_index--;
                    }
                }
            }
            break;
        }
    }
    return false;
}

Widget * List::childAt(uint16_t index)
{
    debug_if(index > m_contentItem->m_container.size(), "Index out of range");
    return m_contentItem->m_container.at(index);
}

void List::push()
{
    switch(m_orientation)
    {
    case FTGUI::List::Vertical:
        if(m_contentItem->x() + m_contentItem->width() <= m_width)
            return;
        m_contentItem->translateX(m_contentItem->x() - m_contentItem->m_container.front()->width());
        break;
    case FTGUI::List::Horizontal:
        if(m_contentItem->y() + m_contentItem->height() <= m_height)
            return;
        m_contentItem->translateY(m_contentItem->y() - m_contentItem->m_container.front()->height());
        break;
    }
    m_index++;
}

void List::pop()
{
    switch(m_orientation)
    {
    case FTGUI::List::Vertical:
        if(m_contentItem->x() >= 0)
            return;
        m_contentItem->translateX(m_contentItem->x() + m_contentItem->m_container.front()->width());
        break;
    case FTGUI::List::Horizontal:
        if(m_contentItem->y() >= 0)
            return;
        m_contentItem->translateY(m_contentItem->y() + m_contentItem->m_container.front()->height());
        break;
    }
    m_index--;
}

void List::setIndex(uint16_t index)
{
    if(m_index == index)
        return;
    if(index >= m_contentItem->m_container.size())
    {
        debug("Index out of Range\n");
        return;
    }
    m_index = index;
    switch(m_orientation)
    {
    case FTGUI::List::Vertical:
        m_contentItem->translateX(m_contentItem->m_container.front()->width() * -index);
        break;
    case FTGUI::List::Horizontal:
        m_contentItem->translateY(m_contentItem->m_container.front()->height() * -index);
        break;
    }
}

uint16_t List::index() const
{
    return m_index;
}

bool List::scrollable() const
{
    return m_scrollable;
}

void List::setScrollable(bool scrollable)
{
    m_scrollable = scrollable;
}

}    // namespace FTGUI
