#include "Nodeui.h"
#include "ConvStr.h"
#include <xutility>


const int CHECK_1 = 1;
static HWND g_hwnd2;
static LONG_PTR g_ori_wproc2;
Nodeui g_Nodeui;

static LRESULT CALLBACK WndProc2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_MOUSEWHEEL:
    {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        if(zDelta > 0)
        {
            g_Nodeui.m_viewer->StepUp();
        }
        else
        {
            g_Nodeui.m_viewer->StepDown();
        }
    }
    break;
    case WM_PAINT:
        if(g_Nodeui.m_viewer)
        {
            g_Nodeui.m_viewer->Render();
        }
        break;
    case WM_MOUSEMOVE:
    {
        int xPos = LOWORD(lParam);
        int yPos = HIWORD(lParam);
        g_Nodeui.m_viewer->MouseMove(xPos, yPos);
    }
    break;
    case WM_LBUTTONDOWN:
    {
		SetFocus( hwnd);
        int xPos = LOWORD(lParam);
        int yPos = HIWORD(lParam);
        g_Nodeui.m_viewer->MouseLDown(xPos, yPos);
    }
    break;
    case WM_LBUTTONUP:
    {
        int xPos = LOWORD(lParam);
        int yPos = HIWORD(lParam);
        g_Nodeui.m_viewer->MouseLUp(xPos, yPos);
    }
    break;
    case WM_SIZE:
    {
        int width = LOWORD(lParam);  // Macro to get the low-order word.
        int height = HIWORD(lParam);
        if(g_Nodeui.m_viewer)
        {
            g_Nodeui.m_viewer->SetSize(width, height);
            g_Nodeui.m_viewer->Render();
        }
    }
    break;
    }
    return CallWindowProc((WNDPROC)g_ori_wproc2, hwnd, msg, wParam, lParam);;
}

static LONG_PTR g_my_wproc2 = (LONG_PTR)WndProc2;

Nodeui::Nodeui(void)
{
}


Nodeui::~Nodeui(void)
{
}


void Nodeui::Render()
{
	return;
    m_viewer->Render();
}

void Nodeui::Init()
{
	return;
    m_num_check = 0;
    w = 600;
    h = 600;
    m_sizeimg.create(h, w, CV_8UC3);
    cv::namedWindow("Nodeui", 0);
    cv::imshow("Nodeui", m_sizeimg);
    g_hwnd2 = (HWND)cvGetWindowHandle("Nodeui");
    g_ori_wproc2 = GetWindowLongPtr(g_hwnd2, GWLP_WNDPROC);
    SetWindowLongPtr(g_hwnd2, GWLP_WNDPROC, g_my_wproc2);
    g_Nodeui.m_viewer = new NodeViewer(g_hwnd2);
    g_Nodeui.m_viewer->SetFontSize(xSize(16, 8));
}
