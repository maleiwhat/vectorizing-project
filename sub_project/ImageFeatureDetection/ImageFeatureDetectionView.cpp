// 這個 MFC 範例原始程式碼會示範如何使用 MFC Microsoft Office Fluent 使用者介面
// (「Fluent UI」)，並且僅提供為參考資料，做為
// MFC 參考及 MFC C++ 程式庫軟體
// 隨附相關電子文件的補充。
// 關於 Fluent UI 之複製、使用或散發的授權條款則分別提供。
// 如需 Fluent UI 授權計劃的詳細資訊，請造訪
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// Copyright (C) Microsoft Corporation
// 著作權所有，並保留一切權利。

// ImageFeatureDetectionView.cpp : CImageFeatureDetectionView 類別的實作
//

#include "stdafx.h"
// SHARED_HANDLERS 可以定義在實作預覽、縮圖和搜尋篩選條件處理常式的
// ATL 專案中，並允許與該專案共用文件程式碼。
#ifndef SHARED_HANDLERS
#include "ImageFeatureDetection.h"
#endif

#include "ImageFeatureDetectionDoc.h"
#include "ImageFeatureDetectionView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CImageFeatureDetectionView* g_MainView;
// CImageFeatureDetectionView

IMPLEMENT_DYNCREATE(CImageFeatureDetectionView, CTabView)

BEGIN_MESSAGE_MAP(CImageFeatureDetectionView, CTabView)
	// 標準列印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW,
			   &CImageFeatureDetectionView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CImageFeatureDetectionView 建構/解構

CImageFeatureDetectionView::CImageFeatureDetectionView()
{
	// TODO: 在此加入建構程式碼
	g_MainView = this;
}

CImageFeatureDetectionView::~CImageFeatureDetectionView()
{
}

BOOL CImageFeatureDetectionView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此經由修改 CREATESTRUCT cs
	// 達到修改視窗類別或樣式的目的
	return CView::PreCreateWindow(cs);
}

// CImageFeatureDetectionView 描繪

void CImageFeatureDetectionView::OnDraw(CDC* /*pDC*/)
{
	CImageFeatureDetectionDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc)
	{ return; }
}


// CImageFeatureDetectionView 列印


void CImageFeatureDetectionView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CImageFeatureDetectionView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 預設的準備列印程式碼
	return DoPreparePrinting(pInfo);
}

void CImageFeatureDetectionView::OnBeginPrinting(CDC* /*pDC*/,
		CPrintInfo* /*pInfo*/)
{
	// TODO: 加入列印前額外的初始設定
}

void CImageFeatureDetectionView::OnEndPrinting(CDC* /*pDC*/,
		CPrintInfo* /*pInfo*/)
{
	// TODO: 加入列印後的清除程式碼
}

void CImageFeatureDetectionView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CImageFeatureDetectionView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y,
			this, TRUE);
#endif
}


// CImageFeatureDetectionView 診斷

#ifdef _DEBUG
void CImageFeatureDetectionView::AssertValid() const
{
	CView::AssertValid();
}

void CImageFeatureDetectionView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImageFeatureDetectionDoc* CImageFeatureDetectionView::GetDocument()
const // 內嵌非偵錯版本
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageFeatureDetectionDoc)));
	return (CImageFeatureDetectionDoc*)m_pDocument;
}

void CImageFeatureDetectionView::SwitchPicture(int index)
{
	SetActiveView(index);
}


#endif //_DEBUG


// CImageFeatureDetectionView 訊息處理常式


int CImageFeatureDetectionView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabView::OnCreate(lpCreateStruct) == -1)
	{ return -1; }

	this->GetTabControl().SetLocation(CMFCTabCtrl:: LOCATION_BOTTOM);
	this->GetTabControl().ModifyTabStyle(CMFCTabCtrl:: STYLE_3D);
	this->GetTabControl().EnableAutoColor(TRUE);
	this->GetTabControl().HideSingleTab(FALSE);
	this->GetTabControl().EnableTabSwap(FALSE);
	this->GetTabControl().SetTabBorderSize(2);
	return 0;
}


void CImageFeatureDetectionView::OnSize(UINT nType, int cx, int cy)
{
	CTabView::OnSize(nType, cx, cy);

	// TODO: 在此加入您的訊息處理常式程式碼

}

void CImageFeatureDetectionView::AddPicturedata(CString name, vavImage* pic,
												int index)
{
	if (m_ViewMap.find(pic) == m_ViewMap.end())
	{
		AddView(RUNTIME_CLASS(CD3DpictureView), name, index);
		g_NewPictureView->m_PictureID = index;
		g_NewPictureView->OnInitialUpdate();
		//g_NewPictureView->Refresh( pic );
		SetActiveView(index);
		m_ViewMap[pic] = g_NewPictureView;
		g_NewPictureView->SetPictureSize(pic->GetWidth(), pic->GetHeight());
		g_NewPictureView->SetTexture(pic->GetDx11Texture(g_NewPictureView->GetDevice(),
			g_NewPictureView->GetDeviceContext()));
	}
}
