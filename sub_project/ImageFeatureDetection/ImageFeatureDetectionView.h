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

// ImageFeatureDetectionView.h : CImageFeatureDetectionView 類別的介面
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
protected: // 僅從序列化建立
	CImageFeatureDetectionView();
	DECLARE_DYNCREATE(CImageFeatureDetectionView)

// 屬性
public:
	CImageFeatureDetectionDoc* GetDocument() const;
	ViewMap m_ViewMap;

// 作業
public:

// 覆寫
public:
	virtual void OnDraw(CDC* pDC);  // 覆寫以描繪此檢視
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 程式碼實作
public:
	virtual ~CImageFeatureDetectionView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
// 產生的訊息對應函式
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


#ifndef _DEBUG  // ImageFeatureDetectionView.cpp 中的偵錯版本
inline CImageFeatureDetectionDoc* CImageFeatureDetectionView::GetDocument()
const
{ return reinterpret_cast<CImageFeatureDetectionDoc*>(m_pDocument); }
#endif

