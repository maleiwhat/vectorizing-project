// �o�� MFC �d�ҭ�l�{���X�|�ܽd�p��ϥ� MFC Microsoft Office Fluent �ϥΪ̤���
// (�uFluent UI�v)�A�åB�ȴ��Ѭ��ѦҸ�ơA����
// MFC �ѦҤ� MFC C++ �{���w�n��
// �H�������q�l��󪺸ɥR�C
// ���� Fluent UI ���ƻs�B�ϥΩδ��o�����v���ګh���O���ѡC
// �p�� Fluent UI ���v�p�����ԲӸ�T�A�гy�X 
// http://go.microsoft.com/fwlink/?LinkId=238214�C
//
// Copyright (C) Microsoft Corporation
// �ۧ@�v�Ҧ��A�ëO�d�@���v�Q�C

// ImageFeatureDetectionView.cpp : CImageFeatureDetectionView ���O����@
//

#include "stdafx.h"
// SHARED_HANDLERS �i�H�w�q�b��@�w���B�Y�ϩM�j�M�z�����B�z�`����
// ATL �M�פ��A�ä��\�P�ӱM�צ@�Τ��{���X�C
#ifndef SHARED_HANDLERS
#include "ImageFeatureDetection.h"
#endif

#include "ImageFeatureDetectionDoc.h"
#include "ImageFeatureDetectionView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImageFeatureDetectionView

IMPLEMENT_DYNCREATE(CImageFeatureDetectionView, CView)

BEGIN_MESSAGE_MAP(CImageFeatureDetectionView, CView)
	// �зǦC�L�R�O
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CImageFeatureDetectionView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CImageFeatureDetectionView �غc/�Ѻc

CImageFeatureDetectionView::CImageFeatureDetectionView()
{
	// TODO: �b���[�J�غc�{���X

}

CImageFeatureDetectionView::~CImageFeatureDetectionView()
{
}

BOOL CImageFeatureDetectionView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �b���g�ѭק� CREATESTRUCT cs 
	// �F��ק�������O�μ˦����ت�

	return CView::PreCreateWindow(cs);
}

// CImageFeatureDetectionView �yø

void CImageFeatureDetectionView::OnDraw(CDC* /*pDC*/)
{
	CImageFeatureDetectionDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �b���[�J��͸�ƪ��yø�{���X
}


// CImageFeatureDetectionView �C�L


void CImageFeatureDetectionView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CImageFeatureDetectionView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �w�]���ǳƦC�L�{���X
	return DoPreparePrinting(pInfo);
}

void CImageFeatureDetectionView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �[�J�C�L�e�B�~����l�]�w
}

void CImageFeatureDetectionView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �[�J�C�L�᪺�M���{���X
}

void CImageFeatureDetectionView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CImageFeatureDetectionView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CImageFeatureDetectionView �E�_

#ifdef _DEBUG
void CImageFeatureDetectionView::AssertValid() const
{
	CView::AssertValid();
}

void CImageFeatureDetectionView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImageFeatureDetectionDoc* CImageFeatureDetectionView::GetDocument() const // ���O�D��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageFeatureDetectionDoc)));
	return (CImageFeatureDetectionDoc*)m_pDocument;
}
#endif //_DEBUG


// CImageFeatureDetectionView �T���B�z�`��
