/*
 * @file pallet.cpp
 * is part of FTGUI Project
 *
 * Copyright (c) 2020 Mikhail Ivanov <masluf@gmail.com>
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
#include "colors.h"

namespace FTGUI
{
const Color & Theme::background() const
{
    return m_background;
}

const Color & Theme::primary() const
{
    return m_primary;
}

const Color & Theme::primaryLight() const
{
    return m_primaryLight;
}

const Color & Theme::primaryDark() const
{
    return m_primaryDark;
}

const Color & Theme::secondary() const
{
    return m_secondary;
}

const Color & Theme::secondaryLight() const
{
    return m_secondaryLight;
}

const Color & Theme::secondaryDark() const
{
    return m_secondaryDark;
}

const Color & Theme::onPrimary() const
{
    return m_onPrimary;
}
}    // namespace FTGUI
