#include "General.h"


xSize::xSize()
    : w(0), h(0)
{
}
xSize::xSize(int _w, int _h)
    : w(_w), h(_h)
{
}
xRect::xRect()
    : x(0), y(0), w(0), h(0)
{
}
xRect::xRect(int _x, int _y, int _w, int _h)
    : x(_x), y(_y), w(_w), h(_h)
{
}

bool xRect::Collision(int _x, int _y)
{
    if(_x >= x && _y >= y && _x <= x + w && _y <= y + h)
    {
        return true;
    }
    return false;
}

Color::Color()
    : r(0), g(0), b(0)
{
}
Color::Color(unsigned char _r, unsigned char _g,
             unsigned char _b)
    : r(_r), g(_g), b(_b)
{
}
xVec2::xVec2()
    : x(0), y(0)
{
}
xVec2::xVec2(int _x, int _y)
    : x(_x), y(_y)
{
}

void VerticalScroll::Init(int x, int y, int w, int h, int maxv, int minv, int v)
{
    m_Frame = xRect(x, y, w, h);
    m_Scroll = m_Frame;
    m_Scroll.w -= 2;
    m_Scroll.x += 1;
    m_MaxValue = maxv;
    m_MinValue = minv;
    m_Value = v;
    m_MouseY = -1;
}

void VerticalScroll::AutoFitScrollWidth(int w, int vw)
{
    m_ViewWidth = vw;
    m_MaxValue = w;
    m_ScrollWidth = int((double(vw + 1) / w) * m_Frame.h);
    if(m_ScrollWidth < MIN_SCROLL_WIDTH)
    {
        m_ScrollWidth = MIN_SCROLL_WIDTH;
    }
    if(m_ScrollWidth > m_Frame.h)
    {
        m_ScrollWidth = m_Frame.h;
    }
    m_Scroll.h = m_ScrollWidth;
}

void VerticalScroll::UpdateValue(int v)
{
    if(v > m_MaxValue)
    {
        v = m_MaxValue;
    }
    if(v < m_MinValue)
    {
        v = m_MinValue;
    }
    m_Value = v;
    double vrange = m_MaxValue - m_MinValue;
    m_Scroll.y = m_Frame.y + int(m_Value / (vrange) * m_Frame.h);
    if((m_Scroll.y - m_Frame.y) > m_Frame.h - m_Scroll.h)
    {
        m_Scroll.y = m_Frame.y + m_Frame.h - m_Scroll.h;
    }
}

void VerticalScroll::MouseDown(int v)
{
    m_MouseY = v;
    m_MouseDownValue = m_Value;
}

int VerticalScroll::MouseMove(int v)
{
    double move = v - m_MouseY;
    m_Value = m_MouseDownValue + int((move / m_Frame.h) * (m_MaxValue - m_MinValue));
    if(m_Value > m_MaxValue - m_ViewWidth + 20)
    {
        m_Value = m_MaxValue - m_ViewWidth + 20;
    }
    if(m_Value < m_MinValue)
    {
        m_Value = m_MinValue;
    }
    return m_Value;
}

void VerticalScroll::StepUp()
{
    UpdateValue(m_Value - 1);
}

void VerticalScroll::StepDown()
{
    UpdateValue(m_Value + 1);
}



void HorizontalScroll::Init(int x, int y, int w, int h, int maxv, int minv, int v)
{
    m_Frame = xRect(x, y, w, h);
    m_Scroll = m_Frame;
    m_Scroll.h -= 2;
    m_Scroll.y += 1;
    m_MaxValue = maxv;
    m_MinValue = minv;
    m_Value = v;
    m_MouseY = -1;
}

void HorizontalScroll::AutoFitScrollWidth(int w, int vw)
{
	m_ViewWidth = vw;
    m_MaxValue = w;
    m_ScrollWidth = int((double(vw + 1) / w) * m_Frame.w);
    if(m_ScrollWidth < MIN_SCROLL_WIDTH)
    {
        m_ScrollWidth = MIN_SCROLL_WIDTH;
    }
    if(m_ScrollWidth > m_Frame.w)
    {
        m_ScrollWidth = m_Frame.w;
    }
    m_Scroll.w = m_ScrollWidth;
}

void HorizontalScroll::UpdateValue(int v)
{
    if(v > m_MaxValue)
    {
        v = m_MaxValue;
    }
    if(v < m_MinValue)
    {
        v = m_MinValue;
    }
    m_Value = v;
    double vrange = m_MaxValue - m_MinValue;
    m_Scroll.x = m_Frame.x + int(m_Value / (vrange) * m_Frame.w);
    if((m_Scroll.x - m_Frame.x) > m_Frame.w - m_Scroll.w)
    {
        m_Scroll.x = m_Frame.x + m_Frame.w - m_Scroll.w;
    }
}

void HorizontalScroll::MouseDown(int v)
{
    m_MouseY = v;
    m_MouseDownValue = m_Value;
}

int HorizontalScroll::MouseMove(int v)
{
    double move = v - m_MouseY;
    m_Value = m_MouseDownValue + int((move / m_Frame.w) * (m_MaxValue - m_MinValue));
    if(m_Value > m_MaxValue - m_ViewWidth)
    {
        m_Value = m_MaxValue - m_ViewWidth;
    }
    if(m_Value < m_MinValue)
    {
        m_Value = m_MinValue;
    }
    return m_Value;
}

void HorizontalScroll::StepUp()
{
    UpdateValue(m_Value - 1);
}

void HorizontalScroll::StepDown()
{
    UpdateValue(m_Value + 1);
}
