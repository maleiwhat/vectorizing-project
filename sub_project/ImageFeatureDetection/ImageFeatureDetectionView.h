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

// ImageFeatureDetectionView.h : CImageFeatureDetectionView ���O������
//

#pragma once
#include <opencv2/opencv.hpp>
#include <map>
#include "D3DpictureView.h"
#include "vavImage.h"

typedef std::map<vavImage*, CD3DpictureView*> ViewMap;

class CImageFeatureDetectionDoc;
class CImageFeatureDetectionView : public CTabView
{
protected: // �ȱq�ǦC�ƫإ�
	CImageFeatureDetectionView();
	DECLARE_DYNCREATE(CImageFeatureDetectionView)

// �ݩ�
public:
	CImageFeatureDetectionDoc* GetDocument() const;
	ViewMap m_ViewMap;

// �@�~
public:

// �мg
public:
	virtual void OnDraw(CDC* pDC);  // �мg�H�yø���˵�
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �{���X��@
public:
	virtual ~CImageFeatureDetectionView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
// ���ͪ��T�������禡
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()

public:
	void AddPicturedata(CString name, vavImage* pic, int index);
	void SwitchPicture(int index);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
extern CImageFeatureDetectionView* g_MainView;


#ifndef _DEBUG  // ImageFeatureDetectionView.cpp ������������
inline CImageFeatureDetectionDoc* CImageFeatureDetectionView::GetDocument()
const
{ return reinterpret_cast<CImageFeatureDetectionDoc*>(m_pDocument); }
#endif

