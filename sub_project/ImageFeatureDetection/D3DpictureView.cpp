// D3DpictureView.cpp : 實作檔
//

#include "stdafx.h"
#include "D3DpictureView.h"
#include "MainFrm.h"
// CD3DpictureView



IMPLEMENT_DYNCREATE(CD3DpictureView, CView)
CD3DpictureView* g_NewPictureView = NULL;

CD3DpictureView::CD3DpictureView():
	m_MButtonDown(false), m_Scale(1), m_LButtonDown(false)
{
	g_NewPictureView = this;
}

CD3DpictureView::~CD3DpictureView()
{
}

BEGIN_MESSAGE_MAP(CD3DpictureView, CView)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSELEAVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CD3DpictureView 診斷

#ifdef _DEBUG
void CD3DpictureView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CD3DpictureView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif
#endif //_DEBUG

// CD3DpictureView 描繪

void CD3DpictureView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此加入描繪程式碼
	m_D3DApp.BuildPoint();
	m_D3DApp.DrawScene();
}

void CD3DpictureView::InitDx11(HWND hWnd)
{
	CRect rect;
	GetWindowRect(&rect);
	m_hWndDX11 = hWnd;
	::ShowWindow(m_hWndDX11, true);
	::UpdateWindow(m_hWndDX11);
	m_D3DApp.initApp(m_hWndDX11, rect.Width(), rect.Height());
	m_D3DApp.BuildShaderFX();
}

// CD3DpictureView 訊息處理常式

void CD3DpictureView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	//InitDx11(this->GetParent()->GetParent()->GetParent()->GetSafeHwnd());
	this->InitDx11(this->GetSafeHwnd());
}

int CD3DpictureView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	// TODO:  在此加入特別建立的程式碼
	return 0;
}

void CD3DpictureView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此加入您的訊息處理常式程式碼
	if (cx > 0 && cy > 0)
	{
		m_D3DApp.OnResize(cx, cy);
	}
}

void CD3DpictureView::OnMouseMove(UINT nFlags, CPoint point)
{
	CView::OnMouseMove(nFlags, point);

	if (m_MButtonDown)
	{
		//printf("%f %f\n", m_LookCenter.x, m_LookCenter.y);
		m_LookCenter.x = m_LookDown.x + point.x - m_MouseDown.x;
		m_LookCenter.y = m_LookDown.y + point.y - m_MouseDown.y;
		m_D3DApp.SetLookCenter(m_LookCenter.x , m_LookCenter.y);
	}

	if (m_LButtonDown)
	{
		D3DXVECTOR3 color;
		color.x = 1;
		color.y = 0;
		color.z = 0.5;
		double realx = (point.x - m_LookCenter.x) / m_Scale - m_LookCenter.x * 0.5;
		double realy = (m_PicH * m_Scale - m_D3DApp.Height() + point.y
		                - m_LookCenter.y) / m_Scale - m_LookCenter.y * 0.5;
		m_D3DApp.SetMousePoint(realx, realy, 10, color);
		m_D3DApp.BuildPoint();
		m_D3DApp.DrawScene();
		printf("px: %3.1f py: %3.1f Center.x %3.1f Center3.y %3.1f\n",
		       realx, realy, m_LookCenter.x, m_LookCenter.y);
	}
}


BOOL CD3DpictureView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if (zDelta > 0)
	{
		m_Scale += 0.1;
	}
	else
	{
		m_Scale -= 0.1;
	}

	m_D3DApp.SetScale(m_Scale);
	m_D3DApp.BuildPoint();
	m_D3DApp.DrawScene();
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CD3DpictureView::OnMouseLeave()
{
	m_D3DApp.SetLookCenter(m_LookCenter.x, m_LookCenter.y);
	m_MButtonDown = false;
	m_MouseUp.x = m_LookDown.x;
	m_MouseUp.y = m_LookDown.y;
	m_LButtonDown = false;
	CView::OnMouseLeave();
}


void CD3DpictureView::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_D3DApp.SetLookCenter(m_LookCenter.x , m_LookCenter.y);
	m_MButtonDown = true;
	m_MouseDown = point;
	m_LookDown = m_LookCenter;
	CView::OnMButtonDown(nFlags, point);
}


void CD3DpictureView::OnMButtonUp(UINT nFlags, CPoint point)
{
	m_D3DApp.SetLookCenter(m_LookCenter.x, m_LookCenter.y);
	m_MButtonDown = false;
	m_MouseUp = point;
	CView::OnMButtonUp(nFlags, point);
}

void CD3DpictureView::OnPaint()
{
	CPaintDC dc(this);   // device context for painting
	CRect rect;
	GetClientRect(&rect);
	m_D3DApp.OnResize(rect.Width(), rect.Height());
	m_D3DApp.BuildPoint();
	m_D3DApp.DrawScene();
}

void CD3DpictureView::Init()
{
}

void CD3DpictureView::Update(int x, int y)
{
}

void CD3DpictureView::PreBuild(int x, int y)
{
}

void CD3DpictureView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_LButtonDown = true;
	CView::OnLButtonDown(nFlags, point);
}


void CD3DpictureView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_LButtonDown = false;
	CView::OnLButtonUp(nFlags, point);
}
