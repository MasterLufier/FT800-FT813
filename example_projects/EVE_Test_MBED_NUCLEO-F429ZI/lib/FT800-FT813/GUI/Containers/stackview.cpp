/*!
 * @file stackview.cpp
 * is part of FTGUI Project
 *
 * @copyright (c) 2020 Mikhail Ivanov <masluf@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include "stackview.h"

namespace FTGUI
{
StackView::StackView(Widget * parent) :
    Widget(parent)
{
    m_name = "StackView";
    m_indexChange.set(1);
}

void StackView::push()
{
    if(m_index < m_container.size() - 1)
    {
        m_index++;
        queue()->call([&]() {
            m_indexChange.wait_all(1, Duration);
            int8_t i = -m_index;
            for(const auto & w : m_container)
            {
                w->translateX(i * (m_width + 1));
                ++i;
            }
            queue()->call_in(AnimationOpt::Duration, [&]() {
                m_indexChange.set(1);
            });
        });
    }
}

void StackView::pop()
{
    if(m_index > 0)
    {
        m_index--;
        queue()->call([&]() {
            m_indexChange.wait_all(1, Duration);
            int8_t i = -m_index;
            for(const auto & w : m_container)
            {
                w->translateX(i * (m_width + 1));
                ++i;
            }
            queue()->call_in(AnimationOpt::Duration, [&]() {
                m_indexChange.set(1);
            });
        });
    }
}

void StackView::setIndex(uint8_t index)
{
    if(m_index == index)
        return;
    m_index = index;
    update();
}

void StackView::addWidget(Widget * widget)
{
    Widget::addWidget(widget);
    int8_t i = -m_index;
    for(const auto & w : m_container)
    {
        w->setGeometry((i * (m_width + 1)),
                       0,
                       m_width,
                       m_height);
        ++i;
    }
}
}    // namespace FTGUI
