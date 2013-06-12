// D3DpictureView.cpp : ��@��
//

#include "stdafx.h"
#include "D3DpictureView.h"
#include "MainFrm.h"
// CD3DpictureView



IMPLEMENT_DYNCREATE(CD3DpictureView, CView)
CD3DpictureView* g_NewPictureView = NULL;

CD3DpictureView::CD3DpictureView()
	: m_TrackMouse(true), m_LMouseHold(false), m_MMouseHold(false),
	  m_CtrlPress(false), m_KeyAPress(false), m_ShiftPress(false),
	  m_Scale(1)
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
END_MESSAGE_MAP()


// CD3DpictureView �yø

void CD3DpictureView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: �b���[�J�yø�{���X
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
	m_D3DApp.buildShaderFX();
}

// CD3DpictureView �T���B�z�`��

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

	// TODO:  �b���[�J�S�O�إߪ��{���X
	return 0;
}

void CD3DpictureView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: �b���[�J�z���T���B�z�`���{���X
	if (cx > 0 && cy > 0)
	{
		m_D3DApp.OnResize(cx, cy);
	}
}

void CD3DpictureView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
	if (m_TrackMouse)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = this->m_hWnd;
		_TrackMouseEvent(&tme);
		m_TrackMouse = false;
	}

	char buff[100];
	//sprintf( buff, "   ��: %d �C: %d  ", point.x, point.y );
	//CString str( buff );
	//( ( CMainFrame* )( this->GetParent()->GetParent()->GetParent()->GetParentFrame() ) )->SetStatus( str );
	CView::OnMouseMove(nFlags, point);
}


BOOL CD3DpictureView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
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

	char buff[100];
//  sprintf( buff, "   ��ܤ�� %.1f%%", m_D3DApp.m_Picture_Scale * 100 );
//  CString str( buff );
	//( ( CMainFrame* )( this->GetParent()->GetParent()->GetParent()->GetParentFrame() ) )->SetStatus( str );
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CD3DpictureView::OnMouseLeave()
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
	m_TrackMouse = true;
	m_MMouseHold = false;
	m_CtrlPress  = false;
	CView::OnMouseLeave();
}


void CD3DpictureView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
	m_MMouseHold = true;
	CView::OnMButtonDown(nFlags, point);
}


void CD3DpictureView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
	m_MMouseHold = false;
	CView::OnMButtonUp(nFlags, point);
}

void CD3DpictureView::OnPaint()
{
	CPaintDC dc(this);   // device context for painting
	// TODO: �b���[�J�z���T���B�z�`���{���X
	// ���n�I�s�Ϥ��T���� CView::OnPaint()
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

// CD3DpictureView �E�_

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

