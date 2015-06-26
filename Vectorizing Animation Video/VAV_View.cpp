// 這個 MFC 範例原始程式碼會示範如何使用 MFC Microsoft Office Fluent 使用者介面
// (「Fluent UI」)，並且僅提供為參考資料，做為
// MFC 參考及 MFC C++ 程式庫軟體
// 隨附相關電子文件的補充。
// 關於 Fluent UI 之複製、使用或散發的授權條款則分別提供。
// 如需 Fluent UI 授權計劃的詳細資訊，請造訪
// http://msdn.microsoft.com/officeui。
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// VAV_View.cpp : VAV_View 類別的實作
//

#include "stdafx.h"
// SHARED_HANDLERS 可以定義在實作預覽、縮圖和搜尋篩選條件處理常式的
// ATL 專案中，並允許與該專案共用文件程式碼。
#ifndef SHARED_HANDLERS
#include "VAV_App.h"
#endif

#include "VAV_Doc.h"
#include "VAV_View.h"
#include "VAV_MainFrm.h"
#include "IFExtenstion.h"
#include "Line.h"
#include "DX11\d3dApp.h"
#include "GenerateDiffusion.h"
#include <boost\timer\timer.hpp>
#include "Nodeui.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// VAV_View

IMPLEMENT_DYNCREATE(VAV_View, CView)

BEGIN_MESSAGE_MAP(VAV_View, CView)
// 標準列印命令
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &VAV_View::OnFilePrintPreview)
    ON_WM_CONTEXTMENU()
    ON_WM_RBUTTONUP()
    ON_WM_SIZE()
    ON_WM_MOUSEHWHEEL()
    ON_WM_MOUSEWHEEL()
    ON_WM_MOUSEMOVE()
    ON_WM_MBUTTONDOWN()
    ON_WM_MBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_KILLFOCUS()
    ON_WM_CAPTURECHANGED()
    ON_WM_MOUSELEAVE()
    ON_WM_NCMOUSEMOVE()
    ON_WM_NCMBUTTONUP()
    ON_WM_LBUTTONUP()
    ON_WM_TIMER()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
    ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

// VAV_View 建構/解構

VAV_View::VAV_View()
{
// TODO: 在此加入建構程式碼
    m_MButtonDown = false;
    m_Scale = 1.f;
// show historgram
//  m_TimerCallback = vtkSmartNew;
//  m_plot = vtkSmartNew;
//  m_TimerCallback->m_plot = m_plot;
//  m_renderWindow = vtkSmartNew;
//  m_TimerCallback->m_renderWindow = m_renderWindow;
//  m_interactor = vtkOnlyNew;
//  m_TimerCallback->m_interactor = m_interactor;
//m_thread = (HANDLE)_beginthreadex(NULL, 0, MyThreadFunc, this, 0, NULL);
}

VAV_View::~VAV_View()
{
}

BOOL VAV_View::PreCreateWindow(CREATESTRUCT& cs)
{
// TODO: 在此經由修改 CREATESTRUCT cs
// 達到修改視窗類別或樣式的目的
    return CView::PreCreateWindow(cs);
}

// VAV_View 描繪

void VAV_View::OnDraw(CDC* /*pDC*/)
{
    VAV_Doc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if(!pDoc)
    {
        return;
    }
// TODO: 在此加入原生資料的描繪程式碼
    m_D3DApp.BuildPoint();
    m_D3DApp.DrawScene();
}


// VAV_View 列印


void VAV_View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
    AFXPrintPreview(this);
#endif
}

BOOL VAV_View::OnPreparePrinting(CPrintInfo* pInfo)
{
// 預設的準備列印程式碼
    return DoPreparePrinting(pInfo);
}

void VAV_View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
// TODO: 加入列印前額外的初始設定
}

void VAV_View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
// TODO: 加入列印後的清除程式碼
}

void VAV_View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
    theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y,
            this, TRUE);
#endif
}

void VAV_View::InitDx11(HWND hWnd)
{
    CRect rect;
    GetWindowRect(&rect);
    m_LookCenter.x = 0;
    m_LookCenter.y = 0;
//  m_hWndDX11 = CreateWindowA("edit", "", WS_CHILD | WS_DISABLED | WS_VISIBLE
//      , 0, 0, rect.right-rect.left, rect.bottom-rect.top, hWnd,
//      (HMENU)"", 0, NULL);
    m_hWndDX11 = hWnd;
    ::ShowWindow(m_hWndDX11, true);
    ::UpdateWindow(m_hWndDX11);
    m_D3DApp.initApp(m_hWndDX11, rect.Width(), rect.Height());
    m_D3DApp.BuildShaderFX();
    m_D3DApp.SetLookCenter(m_LookCenter.x , m_LookCenter.y);
    vavImage::SetDx11Device(m_D3DApp.GetDevice(), m_D3DApp.GetDeviceContext());
}

VAV_MainFrame* VAV_View::GetMainFrame()
{
    return ((VAV_MainFrame*)GetParentFrame());
}

// VAV_View 診斷

#ifdef _DEBUG
void VAV_View::AssertValid() const
{
    CView::AssertValid();
}

void VAV_View::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

VAV_Doc* VAV_View::GetDocument() const // 內嵌非偵錯版本
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(VAV_Doc)));
    return (VAV_Doc*)m_pDocument;
}
#endif //_DEBUG


// VAV_View 訊息處理常式

void VAV_View::OnInitialUpdate()
{
    CView::OnInitialUpdate();
    InitDx11(GetSafeHwnd());
}

void VAV_View::SetTexture(ID3D11ShaderResourceView* tex)
{
    m_D3DApp.SetScale(m_Scale);
    m_D3DApp.SetTexture(tex);
    m_D3DApp.BuildPoint();
    m_D3DApp.DrawScene();
    m_ExpImage = GetImage()->Clone();
    m_ExpImage.ToExpImage();
}


void VAV_View::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);
    m_D3DApp.OnResize(cx, cy);
}

void VAV_View::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
    CView::OnMouseHWheel(nFlags, zDelta, pt);
}

BOOL VAV_View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    if(zDelta > 0)
    {
        m_Scale += 0.5;
        printf("m_Scale %f\n", m_Scale);
    }
    else if(zDelta < 0)
    {
        m_Scale -= 0.25;
        printf("m_Scale %f\n", m_Scale);
    }
    m_D3DApp.SetScale(m_Scale);
    m_D3DApp.DrawScene();
//cv::imwrite("draw.png", m_D3DApp.DrawSceneToCvMat(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)));
    return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void VAV_View::OnMouseMove(UINT nFlags, CPoint point)
{
    CView::OnMouseMove(nFlags, point);
    if(m_MButtonDown)
    {
//printf("%f %f\n", m_LookCenter.x, m_LookCenter.y);
        m_LookCenter.x = m_LookDown.x + point.x - m_MouseDown.x;
        m_LookCenter.y = m_LookDown.y + point.y - m_MouseDown.y;
        m_D3DApp.SetLookCenter(m_LookCenter.x , m_LookCenter.y);
    }
    if(m_LButtonDown)
    {
        m_MouseMove = point;
        ShowLineNormal();
    }
}

void VAV_View::OnMButtonDown(UINT nFlags, CPoint point)
{
    CView::OnMButtonDown(nFlags, point);
}

void VAV_View::OnMButtonUp(UINT nFlags, CPoint point)
{
    CView::OnMButtonUp(nFlags, point);
}

void VAV_View::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_LButtonDown = true;
    m_MouseMove = point;
    CView::OnLButtonDown(nFlags, point);
    ShowLineNormal();
    double realX = (m_MouseMove.x - m_LookCenter.x) / m_Scale - m_LookCenter.x * 0.5;
    double realY = (m_PicH * m_Scale - m_D3DApp.GetHeight() + m_MouseMove.y
                    - m_LookCenter.y) / m_Scale - m_LookCenter.y * 0.5;
    if(realX > 0 && realY > 0 && realX < m_indexImg.cols && realY < m_indexImg.rows)
    {
        cv::Vec3b p = m_indexImg.at<cv::Vec3b>(realY * 2, realX * 2);
        int idx = p[0] + p[1] * 256 + p[2] * 256 * 256;
        printf("%d |", idx);
    }
    int x1 = g_Nodeui.m_viewer->m_Focus1;
    int x2 = g_Nodeui.m_viewer->m_Focus2;
    int fr = g_Nodeui.m_viewer->m_HScrollF_x;
    FrameInfos& fgframes = GetMainFrame()->m_FrameInfos;
    if(fgframes.size() > 0)
    {
        int region = fgframes[fr].picmesh1.GetRegionId(realX, realY);
        if(region >= 0)
        {
            g_Nodeui.m_viewer->m_Focus1 = fr;
            g_Nodeui.m_viewer->m_Focus2 = region;
            g_Nodeui.m_viewer->NeedRender();
        }
    }

    printf("\n");
}
vavImage* VAV_View::GetImage()
{
    return &(GetMainFrame()->m_vavImage);
}
void VAV_View::OnKillFocus(CWnd* pNewWnd)
{
    CView::OnKillFocus(pNewWnd);
// TODO: 在此加入您的訊息處理常式程式碼
    printf("OnMouseLeave\n");
}
void VAV_View::OnCaptureChanged(CWnd* pWnd)
{
// TODO: 在此加入您的訊息處理常式程式碼
    CView::OnCaptureChanged(pWnd);
    printf("OnMouseLeave\n");
}
void VAV_View::OnMouseLeave()
{
// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
    CView::OnMouseLeave();
    printf("OnMouseLeave\n");
}
void VAV_View::OnNcMouseMove(UINT nHitTest, CPoint point)
{
// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
    CView::OnNcMouseMove(nHitTest, point);
    printf("OnNcMouseMove\n");
    m_D3DApp.SetLookCenter(m_LookCenter.x, m_LookCenter.y);
    m_MButtonDown = false;
    m_MouseUp.x = m_LookDown.x;
    m_MouseUp.y = m_LookDown.y;
}
void VAV_View::OnNcMButtonUp(UINT nHitTest, CPoint point)
{
// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
    CView::OnNcMButtonUp(nHitTest, point);
    printf("OnNcMButtonUp\n");
    m_D3DApp.SetLookCenter(m_LookCenter.x, m_LookCenter.y);
    m_MButtonDown = false;
    m_MouseUp.x = m_LookDown.x;
    m_MouseUp.y = m_LookDown.y;
}
D3DApp& VAV_View::GetD3DApp()
{
    return m_D3DApp;
}

void VAV_View::SetPictureSize(int w, int h)
{
    m_PicW = w;
    m_PicH = h;
    m_D3DApp.SetPictureSize(w, h);
}

void VAV_View::OnLButtonUp(UINT nFlags, CPoint point)
{
// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
    CView::OnLButtonUp(nFlags, point);
    m_LButtonDown = false;
}

void VAV_View::ShowLineNormal()
{
    double realX = (m_MouseMove.x - m_LookCenter.x) / m_Scale - m_LookCenter.x *
                   0.5;
    double realY = (m_PicH * m_Scale - m_D3DApp.GetHeight() + m_MouseMove.y
                    - m_LookCenter.y) / m_Scale - m_LookCenter.y * 0.5;
//printf("%3.2f %3.2f\n", realX, realY);
    m_D3DApp.SetLookCenter(m_LookCenter.x, m_LookCenter.y);
    Vector2 click(realX, realY);
    int idx1 = -1;
    int idx2 = -1;
    double dis = 5;
    int i = 0, j;
    for(auto it1 = m_FeatureLines.begin(); it1 != m_FeatureLines.end(); ++it1, ++i)
    {
        j = 0;
        auto tend = --(it1->end());
        for(auto it2 = it1->begin(); it2 != tend; ++it2, ++j)
        {
            double nowDis = click.squaredDistance(*it2);
            if(nowDis < dis)
            {
                idx1 = i;
                idx2 = j;
                dis = nowDis;
            }
        }
    }
}

unsigned __stdcall VAV_View::MyThreadFunc(LPVOID lpParam)
{
    return 0;
}

void VAV_View::OnTimer(UINT_PTR nIDEvent)
{
    int x1 = g_Nodeui.m_viewer->m_Focus1;
    int x2 = g_Nodeui.m_viewer->m_Focus2;
    int fr = g_Nodeui.m_viewer->m_HScrollF_x;
    if(0 && x1 != -1)
    {
        m_D3DApp.ClearTriangles();
        FrameInfos& fgframes = GetMainFrame()->m_FrameInfos;
        ints& tmps = fgframes[x1 - 1].picmesh1.m_MapingRegionIDs;
        int idx = std::find(tmps.begin(), tmps.end(), x2) - tmps.begin();
        fgframes[x1 - 1].picmesh1.MakeColorX9(idx);
        ColorTriangles ctsx = fgframes[x1 - 1].picmesh1.m_Trangles;
        m_D3DApp.AddColorTriangles(ctsx);
        m_D3DApp.AddTrianglesLine(ctsx);
        m_D3DApp.BuildPoint();
        m_D3DApp.DrawScene();
    }
    if(1)
    {
        Vec2fs& moves = GetMainFrame()->m_Moves;
        m_D3DApp.ClearTriangles();
        BackGround& dframes = GetMainFrame()->m_BackGround;
        FrameInfos& fgframes = GetMainFrame()->m_FrameInfos;
        static ColorTriangles c1, c2;
        static int i = 1;
        static int lasti = -1;
        static int playidx = -1;
        const int LEN = fgframes.size() - 3;
        const int LEN2 = 1;
        playidx++;
        if(playidx >= LEN2)
        {
            playidx = 0;
            i++;
        }
        if(i >= LEN)
        {
            i = 0;
        }
        if(lasti != i)
        {
            lasti = i;
            //fgframes[i].picmesh1.GetMappingCT(fgframes[i + 1].picmesh1, c1, c2, -moves[i+1][0], -moves[i+1][1]);
        }
        //printf("i %d playi %d\n", i, playidx);
		Lines bgline = fgframes.back().curves1;
		
        {
            //ColorTriangles ctsx = fgframes[i].picmesh1.Interpolation(c1, c2, (float)playidx / LEN2);
            ColorTriangles ctsx;
            cv::Vec2f& move = GetMainFrame()->m_Moves[i];
            float movex = GetMainFrame()->movex;
            float movey = GetMainFrame()->movey;
//             fgframes.back().picmesh1.MakeColorX2(
//                 fgframes[i].picmesh1.m_MapingRegionIDs, movex + move[0] + 2, movey + move[1]);
            ctsx = fgframes.back().picmesh1.m_Trangles;
			for (int x=0;x<ctsx.size();++x)
			{
				ctsx[x].pts[0].x += movex + move[0] + 2;
				ctsx[x].pts[1].x += movex + move[0] + 2;
				ctsx[x].pts[2].x += movex + move[0] + 2;
			}
            m_D3DApp.AddColorTriangles(ctsx);
            //m_D3DApp.AddTrianglesLine(ctsx);
			bgline = GetLines(bgline, movex + move[0] + 2, 0);
			m_D3DApp.AddLinesWidth(bgline, fgframes.back().tmplinewidth, fgframes.back().ocolor1);
			ctsx = fgframes[i].picmesh1.m_Trangles;
			m_D3DApp.AddLayer();
			m_D3DApp.AddColorTriangles(ctsx);
			m_D3DApp.AddTrianglesLine(ctsx);
			m_D3DApp.AddLinesWidth(fgframes[i].picmesh1.fglines, 
				fgframes[i].picmesh1.fglinesW, 
				fgframes[i].picmesh1.fglineColors);
			
            m_D3DApp.BuildPoint();
            m_D3DApp.DrawScene();
        }
    }
}


void VAV_View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if(nChar == 17)
    {
        m_HoldCtrl = true;
    }
    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void VAV_View::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if(nChar == 17)
    {
        m_HoldCtrl = false;
    }
    CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


void VAV_View::OnRButtonDown(UINT nFlags, CPoint point)
{
    CView::OnRButtonDown(nFlags, point);
    m_D3DApp.SetLookCenter(m_LookCenter.x , m_LookCenter.y);
    m_MButtonDown = true;
    m_MouseDown = point;
    m_LookDown = m_LookCenter;
}

void VAV_View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
//  ClientToScreen(&point);
//  OnContextMenu(this, point);
    m_D3DApp.SetLookCenter(m_LookCenter.x, m_LookCenter.y);
    m_MButtonDown = false;
    m_MouseUp = point;
}
