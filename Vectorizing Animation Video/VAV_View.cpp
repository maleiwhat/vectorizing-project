// �o�� MFC �d�ҭ�l�{���X�|�ܽd�p��ϥ� MFC Microsoft Office Fluent �ϥΪ̤���
// (�uFluent UI�v)�A�åB�ȴ��Ѭ��ѦҸ�ơA����
// MFC �ѦҤ� MFC C++ �{���w�n��
// �H�������q�l��󪺸ɥR�C
// ���� Fluent UI ���ƻs�B�ϥΩδ��o�����v���ګh���O���ѡC
// �p�� Fluent UI ���v�p�����ԲӸ�T�A�гy�X
// http://msdn.microsoft.com/officeui�C
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// VAV_View.cpp : VAV_View ���O����@
//

#include "stdafx.h"
// SHARED_HANDLERS �i�H�w�q�b��@�w���B�Y�ϩM�j�M�z�����B�z�`����
// ATL �M�פ��A�ä��\�P�ӱM�צ@�Τ��{���X�C
#ifndef SHARED_HANDLERS
#include "VAV_App.h"
#endif

#include "VAV_Doc.h"
#include "VAV_View.h"
#include "VAV_MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// VAV_View

IMPLEMENT_DYNCREATE(VAV_View, CView)

BEGIN_MESSAGE_MAP(VAV_View, CView)
	// �зǦC�L�R�O
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
END_MESSAGE_MAP()

// VAV_View �غc/�Ѻc

VAV_View::VAV_View()
{
	// TODO: �b���[�J�غc�{���X
	m_MButtonDown = false;
	m_Scale = 1.f;
}

VAV_View::~VAV_View()
{
}

BOOL VAV_View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �b���g�ѭק� CREATESTRUCT cs
	// �F��ק�������O�μ˦����ت�
	return CView::PreCreateWindow(cs);
}

// VAV_View �yø

void VAV_View::OnDraw(CDC* /*pDC*/)
{
	VAV_Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc)
	{
		return;
	}

	// TODO: �b���[�J��͸�ƪ��yø�{���X
	m_D3DApp.BuildPoint();
	m_D3DApp.DrawScene();
}


// VAV_View �C�L


void VAV_View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL VAV_View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �w�]���ǳƦC�L�{���X
	return DoPreparePrinting(pInfo);
}

void VAV_View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �[�J�C�L�e�B�~����l�]�w
}

void VAV_View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �[�J�C�L�᪺�M���{���X
}

void VAV_View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void VAV_View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

void VAV_View::InitDx11(HWND hWnd)
{
	CRect rect;
	GetWindowRect(&rect);
	m_LookCenter.x = 0;
	m_LookCenter.y = 0;
	// 	m_hWndDX11 = CreateWindowA("edit", "", WS_CHILD | WS_DISABLED | WS_VISIBLE
	// 		, 0, 0, rect.right-rect.left, rect.bottom-rect.top, hWnd,
	// 		(HMENU)"", 0, NULL);
	m_hWndDX11 = hWnd;
	::ShowWindow(m_hWndDX11, true);
	::UpdateWindow(m_hWndDX11);
	m_D3DApp.initApp(m_hWndDX11, rect.Width(), rect.Height());
	m_D3DApp.BuildShaderFX();
	m_D3DApp.SetLookCenter(m_LookCenter.x , m_LookCenter.y);
	vavImage::SetDx11Device(m_D3DApp.GetDevice());
}

VAV_MainFrame* VAV_View::GetMainFrame()
{
	return ((VAV_MainFrame*)GetParentFrame());
}

// VAV_View �E�_

#ifdef _DEBUG
void VAV_View::AssertValid() const
{
	CView::AssertValid();
}

void VAV_View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

VAV_Doc* VAV_View::GetDocument() const // ���O�D��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(VAV_Doc)));
	return (VAV_Doc*)m_pDocument;
}
#endif //_DEBUG


// VAV_View �T���B�z�`��

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
	if (zDelta > 0)
	{
		m_Scale += 0.5;
		printf("m_Scale %f\n", m_Scale);
	}
	else if (zDelta < 0)
	{
		m_Scale -= 0.1;
		printf("m_Scale %f\n", m_Scale);
	}

	m_D3DApp.SetScale(m_Scale);
	m_D3DApp.DrawScene();
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void VAV_View::OnMouseMove(UINT nFlags, CPoint point)
{
	CView::OnMouseMove(nFlags, point);

	if (m_MButtonDown)
	{
		//printf("%f %f\n", m_LookCenter.x, m_LookCenter.y);
		m_LookCenter.x = m_LookDown.x + point.x - m_MouseDown.x;
		m_LookCenter.y = m_LookDown.y + point.y - m_MouseDown.y;
		m_D3DApp.SetLookCenter(m_LookCenter.x , m_LookCenter.y);
	}
}


void VAV_View::OnMButtonDown(UINT nFlags, CPoint point)
{
	CView::OnMButtonDown(nFlags, point);
	m_D3DApp.SetLookCenter(m_LookCenter.x , m_LookCenter.y);
	m_MButtonDown = true;
	m_MouseDown = point;
	m_LookDown = m_LookCenter;
}


void VAV_View::OnMButtonUp(UINT nFlags, CPoint point)
{
	CView::OnMButtonUp(nFlags, point);
	m_D3DApp.SetLookCenter(m_LookCenter.x, m_LookCenter.y);
	m_MButtonDown = false;
	m_MouseUp = point;
}


void VAV_View::OnLButtonDown(UINT nFlags, CPoint point)
{
	CView::OnLButtonDown(nFlags, point);
// 	printf("%3d %3d ", point.x, m_D3DApp.Height()-point.y);
// 	printf("%3d %3d %3.2f %3.2f %3.2f\n", GetImage()->GetHeight(), m_D3DApp.Height(), m_Scale, m_LookCenter.x, m_LookCenter.y);
	printf("%3.2f %3.2f\n", (point.x-m_LookCenter.x)/m_Scale, (GetImage()->GetHeight()*m_Scale-m_D3DApp.Height()+point.y-m_LookCenter.y)/m_Scale);
	GetMainFrame()->ShowPatch((point.x-m_LookCenter.x)/m_Scale, (GetImage()->GetHeight()*m_Scale-m_D3DApp.Height()+point.y-m_LookCenter.y)/m_Scale);
	m_D3DApp.SetLookCenter(m_LookCenter.x, m_LookCenter.y);
}

vavImage* VAV_View::GetImage()
{
	return &(GetMainFrame()->m_vavImage);
}


void VAV_View::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);

	// TODO: �b���[�J�z���T���B�z�`���{���X
	printf("OnMouseLeave\n");
}


void VAV_View::OnCaptureChanged(CWnd *pWnd)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X

	CView::OnCaptureChanged(pWnd);
	printf("OnMouseLeave\n");
}


void VAV_View::OnMouseLeave()
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��

	CView::OnMouseLeave();
	printf("OnMouseLeave\n");
	
}


void VAV_View::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��

	CView::OnNcMouseMove(nHitTest, point);
	printf("OnNcMouseMove\n");
	m_D3DApp.SetLookCenter(m_LookCenter.x, m_LookCenter.y);
	m_MButtonDown = false;
	m_MouseUp.x = m_LookDown.x;
	m_MouseUp.y = m_LookDown.y;
}


void VAV_View::OnNcMButtonUp(UINT nHitTest, CPoint point)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��

	CView::OnNcMButtonUp(nHitTest, point);
	printf("OnNcMButtonUp\n");
	m_D3DApp.SetLookCenter(m_LookCenter.x, m_LookCenter.y);
	m_MButtonDown = false;
	m_MouseUp.x = m_LookDown.x;
	m_MouseUp.y = m_LookDown.y;
}
