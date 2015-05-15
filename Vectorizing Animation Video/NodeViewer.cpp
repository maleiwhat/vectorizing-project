#include "NodeViewer.h"

static const int GAP_H = 30;
xSize NodeViewer::GetSize()
{
    return m_Size;
}

void NodeViewer::SetSize(int w, int h)
{
    RECT rc;
    GetWindowRect(m_hwnd, &rc);
    HDC hdcWindow = GetDC(m_hwnd);
    m_Size.w = w;
    m_Size.h = h;
    m_VerticalScroll.Init(m_Size.w - m_MarginSize.right,
                          m_MarginSize.top,
                          m_MarginSize.right,
                          m_Size.h - m_MarginSize.bottom - m_MarginSize.top,
                          10, 0, 0);
    m_HorizontalScroll.Init(m_MarginSize.left,
                            m_Size.h - m_MarginSize.bottom,
                            m_Size.w - m_MarginSize.left - m_MarginSize.right,
                            m_Size.h,
                            10, 0, 0);
    m_HScroll_Frames.Init(m_MarginSize.left,
                          0,
                          m_Size.w - m_MarginSize.left - m_MarginSize.right,
                          20,
                          10, 0, 0);
    // make back buffer
    GetClientRect(m_hwnd, &m_rcSize);
    m_hdcBackBuffer = CreateCompatibleDC(hdcWindow);
    m_hbmBackBuffer = CreateCompatibleBitmap(hdcWindow,
                      m_rcSize.right - m_rcSize.left, m_rcSize.bottom - m_rcSize.top);
    SelectObject(m_hdcBackBuffer, m_hbmBackBuffer);  // SHOULD SAVE PREVIOUS...
    SelectObject(m_hdcBackBuffer, m_hfont);
    ReleaseDC(m_hwnd, hdcWindow);
}

NodeViewer::NodeViewer(HWND hwnd): m_hwnd(hwnd)
{
    // set Margin Size
    m_MarginSize.left = 5;
    m_MarginSize.right = 10;
    m_MarginSize.top = 5;
    m_MarginSize.bottom = 10;

    RECT rc;
    GetWindowRect(m_hwnd, &rc);
    HDC hdcWindow = GetDC(hwnd);
    SetSize(rc.right - rc.left, rc.bottom - rc.top);
    // make back buffer
    GetClientRect(hwnd, &m_rcSize);
    m_hdcBackBuffer = CreateCompatibleDC(hdcWindow);
    m_hbmBackBuffer = CreateCompatibleBitmap(hdcWindow,
                      m_rcSize.right - m_rcSize.left, m_rcSize.bottom - m_rcSize.top);
    SelectObject(m_hdcBackBuffer, m_hbmBackBuffer);  // SHOULD SAVE PREVIOUS...
    ReleaseDC(hwnd, hdcWindow);
    m_hfont = CreateFont(16, 8, 0, 0,
                         FW_NORMAL, FALSE, FALSE, FALSE,
                         ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                         CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                         DEFAULT_PITCH ,
                         L"Courier New");
    SelectObject(hdcWindow, m_hfont);
    HFONT h_reg_font = (HFONT)GetCurrentObject(hdcWindow, OBJ_FONT);
    LOGFONT font = { 0 };
    GetObject(h_reg_font, sizeof(LOGFONT), &font);
    m_FontSize.h = font.lfHeight;
    m_FontSize.w = font.lfWidth;
    // 轉中文字用
    setlocale(LC_ALL, "");
    m_Lines = 0;
//     for(int i = 0; i < 30; ++i)
//  {
//      AddNodeLine(rand() % 50);
//  }
//  for(int i = 0; i < 30; ++i)
//  {
//      AddNodeLink(rand() % m_DrawNodes.size(), rand() % m_DrawNodes.size());
//  }
    m_LDownVScroll = false;
    m_LDownHScroll = false;
    m_LDownHScrollF = false;
    m_maxFrame = 10;
    m_Focus1 = -1;
    m_BackGroundColor = Color(100, 100, 100);
    m_maxElement = 1;
    m_HScrollF_x = 0;
}


NodeViewer::~NodeViewer(void)
{
}

void NodeViewer::DrawRect(const xRect& area, const Color& c)
{
    RECT draw_rc;
    draw_rc.left = area.x;
    draw_rc.right = area.x + area.w;
    draw_rc.top = area.y;
    draw_rc.bottom = area.y + area.h;
    COLORREF crBkgnd = RGB(c.r, c.g, c.b);
    HBRUSH hbrBkgnd = CreateSolidBrush(crBkgnd);
    FillRect(m_hdcBackBuffer, &draw_rc, hbrBkgnd);
    DeleteObject(hbrBkgnd);
}

void NodeViewer::DrawLeftString(int x, int y, const std::wstring& s)
{
    RECT rcText;
    rcText.left = x;
    rcText.top = y;
    rcText.right = x + 2000;
    rcText.bottom = y + 100;
    SetBkMode(m_hdc, TRANSPARENT);
    DrawText(m_hdc, s.c_str(), (int)s.length(), &rcText,
             DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
}

void NodeViewer::SetFontSize(const xSize& s)
{
    HFONT h_reg_font = (HFONT)GetCurrentObject(m_hdcBackBuffer, OBJ_FONT);
    LOGFONT logFont = { 0 };
    GetObject(h_reg_font, sizeof(LOGFONT), &logFont);
    logFont.lfHeight = s.w; //rounded to nearest int
    logFont.lfWidth = s.h;
    h_reg_font = CreateFontIndirect(&logFont);
    SelectObject(m_hdcBackBuffer, h_reg_font);
    m_FontSize.w = 0;
    m_FontSize.h = 0;
    GetFontSize();
}

xSize NodeViewer::GetFontSize()
{
    if(m_FontSize.h == 0)
    {
        HFONT h_reg_font = (HFONT)GetCurrentObject(m_hdcBackBuffer, OBJ_FONT);
        LOGFONT font = { 0 };
        GetObject(h_reg_font, sizeof(LOGFONT), &font);
        m_FontSize.h = font.lfHeight;
        m_FontSize.w = font.lfWidth;
    }
    return m_FontSize;
}

void NodeViewer::InterRenderStart()
{
    m_hdc = BeginPaint(m_hwnd, &m_ps);
    m_hdc = m_hdcBackBuffer;
    // compute some variable
    int start_y = m_MarginSize.top;
    int end_y = m_Size.h - m_MarginSize.bottom - m_MarginSize.top;
}

void NodeViewer::InterRenderEnd()
{
    BitBlt(m_ps.hdc, 0, 0, m_rcSize.right - m_rcSize.left, m_rcSize.bottom - m_rcSize.top,
           m_hdcBackBuffer, 0, 0, SRCCOPY);
    EndPaint(m_hwnd, &m_ps);
}


void NodeViewer::InterRenderBackGround()
{
    xRect all(0, 0, m_Size.w, m_Size.h);
    xRect inter(m_MarginSize.left, m_MarginSize.top,
                m_Size.w - m_MarginSize.right - m_MarginSize.left,
                m_Size.h - m_MarginSize.bottom - m_MarginSize.top);
    //DrawRect(all, m_MarginColor);
    DrawRect(all, m_BackGroundColor);
}

void NodeViewer::Render()
{
    // 畫畫開始
    InterRenderStart();
    // 先畫背景
    InterRenderBackGround();
    // 畫出node
    InterRenderNode();
    InterRenderLine();
    // 畫出滾動條
    DrawScroll();
    // 畫畫結束
    InterRenderEnd();
}

void NodeViewer::DrawScroll()
{
    m_VerticalScroll.AutoFitScrollWidth(m_Lines * GAP_H, m_Size.h);
    m_VerticalScroll.UpdateValue(m_Offset.y);
    DrawRect(m_VerticalScroll.m_Frame, Color(255, 0, 0));
    DrawRect(m_VerticalScroll.m_Scroll, Color(0, 180, 0));
    m_HorizontalScroll.AutoFitScrollWidth(m_maxElement * 20, m_Size.w);
    m_HorizontalScroll.UpdateValue(m_Offset.x);
    DrawRect(m_HorizontalScroll.m_Frame, Color(255, 0, 0));
    DrawRect(m_HorizontalScroll.m_Scroll, Color(0, 180, 0));

    m_HScroll_Frames.AutoFitScrollWidth(m_maxFrame, 1);
    m_HScroll_Frames.UpdateValue(m_HScrollF_x);
    DrawRect(m_HScroll_Frames.m_Frame, Color(240, 240, 240));
    DrawRect(m_HScroll_Frames.m_Scroll, Color(180, 180, 180));
}


void NodeViewer::MouseLDown(int x, int y)
{
    m_LDownPosition.x = x;
    m_LDownPosition.y = y;
    if(m_VerticalScroll.m_Frame.Collision(x, y))
    {
        m_VerticalScroll.MouseDown(y);
        m_LDownVScroll = true;
    }
    if(m_HorizontalScroll.m_Frame.Collision(x, y))
    {
        m_HorizontalScroll.MouseDown(x);
        m_LDownHScroll = true;
    }
    if(m_HScroll_Frames.m_Frame.Collision(x, y))
    {
        m_HScroll_Frames.MouseDown(x);
        m_LDownHScrollF = true;
    }
    m_Focus1 = -1;
    x += m_Offset.x;
    y += m_Offset.y;
    if(!m_LDownVScroll && !m_LDownHScroll)
    {
        for(int i = 0; i < m_DrawNodes.size(); ++i)
        {
            if(m_DrawNodes[i].m_Pos.Collision(x, y))
            {
                m_Focus1 = m_DrawNodes[i].m_Id / 1000;
                m_Focus2 = m_DrawNodes[i].m_Id % 1000;
                printf("m_Focus1 %d m_Focus2 %d\n", m_Focus1, m_Focus2);
                break;
            }
        }
    }
    InvalidateRect(m_hwnd, NULL, TRUE);
}

void NodeViewer::MouseMove(int x, int y)
{
    if(m_LDownVScroll)
    {
        m_Offset.y = m_VerticalScroll.MouseMove(y);
    }
    if(m_LDownHScroll)
    {
        m_Offset.x = m_HorizontalScroll.MouseMove(x);
    }
    if(m_LDownHScrollF)
    {
        m_HScrollF_x = m_HScroll_Frames.MouseMove(x);
    }
    InvalidateRect(m_hwnd, NULL, TRUE);
}

void NodeViewer::MouseLUp(int x, int y)
{
    m_LDownVScroll = false;
    m_LDownHScroll = false;
    m_LDownHScrollF = false;
}

void NodeViewer::AddNodeLine(int n)
{
    if(n > m_maxElement)
    {
        m_maxElement = n;
    }
    for(int i = 0; i < n; ++i)
    {
        DrawNode dn;
        dn.m_Id = m_Lines * 1000 + i;
        dn.m_Pos = xRect(i * 20, (m_Lines + 1) * GAP_H, 10, 10);
        m_DrawNodes.push_back(dn);
    }
    m_Lines++;
}

void NodeViewer::InterRenderNode()
{
    for(int i = 0; i < m_DrawNodes.size(); ++i)
    {
        xRect tmp = m_DrawNodes[i].m_Pos;
        tmp.y -= m_Offset.y;
        tmp.x -= m_Offset.x;
        if((m_Focus1 * 1000 + m_Focus2) == m_DrawNodes[i].m_Id)
        {
            DrawRect(tmp, Color(255, 100, 100));
        }
        else if(m_Focus2 == m_DrawNodes[i].m_Id % 1000)
        {
            DrawRect(tmp, Color(100, 200, 100));
        }
        else
        {
            DrawRect(tmp, Color(200, 200, 200));
        }
    }
}

void NodeViewer::AddNodeLink(int n1, int n2)
{
    xVec2 v1, v2;
    int x = 0;
    for(int i = 0; i < m_DrawNodes.size(); ++i)
    {
        if(m_DrawNodes[i].m_Id == n1)
        {
            v1.x = m_DrawNodes[i].m_Pos.x + 5;
            v1.y = m_DrawNodes[i].m_Pos.y + 5;
            x++;
            if(x == 2)
            {
                break;
            }
        }
        if(m_DrawNodes[i].m_Id == n2)
        {
            v2.x = m_DrawNodes[i].m_Pos.x + 5;
            v2.y = m_DrawNodes[i].m_Pos.y + 5;
            x++;
            if(x == 2)
            {
                break;
            }
        }
    }
    if(x == 2)
    {
        m_Pair1.push_back(v1);
        m_Pair2.push_back(v2);
    }
}

void NodeViewer::InterRenderLine()
{
    HPEN hPen;
    hPen = CreatePen(PS_DOT, 2, RGB(0, 0, 0));
    SelectObject(m_hdc, hPen);
    for(int i = 0; i < m_Pair1.size(); ++i)
    {
        MoveToEx(m_hdcBackBuffer, m_Pair1[i].x - m_Offset.x, m_Pair1[i].y - m_Offset.y, NULL);
        LineTo(m_hdcBackBuffer, m_Pair2[i].x - m_Offset.x, m_Pair2[i].y - m_Offset.y);
    }
    DeleteObject(hPen);
}

void NodeViewer::AddLink(std::vector<int>& v1, std::vector<int>& v2)
{
    for(int i = 0; i < v1.size(); ++i)
    {
        for(int j = 0; j < v2.size(); ++j)
        {
            if(v1[i] == v2[j])
            {
                //printf("%d %d %d %d\n", i, v1[i], j, v2[j]);
                AddNodeLink((m_Lines - 2) * 1000 + v1[i], (m_Lines - 1) * 1000 + v2[j]);
            }
        }
    }
}
void NodeViewer::StepUp()
{
    m_Offset.y -= 10;
}

void NodeViewer::StepDown()
{
    m_Offset.y += 10;
}

void NodeViewer::NeedRender()
{
    InvalidateRect(m_hwnd, NULL, TRUE);
}
