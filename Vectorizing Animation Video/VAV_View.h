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

// VAV_View.h : VAV_View ���O������
//

#pragma once
#include "DX11/d3dApp.h"
#include "math/Vector2.h"

class VAV_Doc;
class VAV_MainFrame;

class VAV_View : public CView
{
protected: // �ȱq�ǦC�ƫإ�
	D3DApp      m_D3DApp;
	VAV_View();
	DECLARE_DYNCREATE(VAV_View)

// �ݩ�
public:
	VAV_Doc* GetDocument() const;

// �@�~
public:
	CPalette    m_cPalette;
	CPalette*   m_pOldPalette;
	CRect       m_oldRect;
	float       m_fRadius;
	CClientDC*  m_pDC;
	float       m_Scale;
	CPoint      m_MouseDown,
	            m_MouseUp,
	            m_MouseMove;
	Vector2     m_LookCenter;
	Vector2     m_LookDown;
	HWND        m_hWndDX11;
	
	bool        m_MButtonDown;
// �мg
public:
	virtual void OnDraw(CDC* pDC);  // �мg�H�yø���˵�
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	D3DApp& GetD3DApp();
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �{���X��@
public:
	virtual ~VAV_View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void InitDx11(HWND hWnd);
	VAV_MainFrame* GetMainFrame();
	vavImage* GetImage();
	void SetTexture(ID3D11ShaderResourceView* tex);
protected:

// ���ͪ��T�������禡
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg void OnMouseLeave();
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcMButtonUp(UINT nHitTest, CPoint point);
};

#ifndef _DEBUG  // VAV_View.cpp ������������
inline VAV_Doc* VAV_View::GetDocument() const
{ return reinterpret_cast<VAV_Doc*>(m_pDocument); }
#endif

