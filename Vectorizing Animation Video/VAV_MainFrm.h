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

// VAV_MainFrm.h : VAV_MainFrame ���O������
//

#pragma once
#include "FileView.h"
#include "ClassView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "vavImage.h"
#include "Patch.h"

class VAV_MainFrame : public CFrameWndEx
{
protected: // �ȱq�ǦC�ƫإ�
	VAV_MainFrame();
	DECLARE_DYNCREATE(VAV_MainFrame)

// �ݩ�
public:
	vavImage    m_vavImage;
	vavImage    m_cannyImage;
	int     m_PatchTransparency;
	int     m_SelectPatchTransparency;
	int     m_BlackRegionThreshold;
	int     m_TriangleLineTransparency;
	int     m_PictureTransparency;
	int     m_LineTransparency;
	int     m_LineSkeletonTransparency;
	CvPatchs    m_CvPatchs;
	void    ShowPatch(double x, double y);
// �@�~
public:
	void OnFileOpenVideo();
	void OnFileOpenPicture();
// �мg
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �{���X��@
public:
	virtual ~VAV_MainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ����C���O������
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	CFileView         m_wndFileView;
	CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;
	CMFCCaptionBar    m_wndCaptionBar;

// ���ͪ��T�������禡
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnViewCaptionBar();
	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	BOOL CreateCaptionBar();
public:
	afx_msg void OnComboColorinterpolation();
	afx_msg void OnButtonCanny();
	afx_msg void OnSpinCannyThreshold1();
	afx_msg void OnSpinCannyThreshold2();
	afx_msg void OnSpinAperturesize();
	afx_msg void OnButtonControlPointInitialize();
	afx_msg void OnButtonCGALTriangulation();
	afx_msg void OnButtonSkeleton();
	afx_msg void OnButtonSobel();
	afx_msg void OnButtonLaplace();
	afx_msg void OnSpinTransparencySelectPatch();
	afx_msg void OnUpdateSpinTransparencySelectPatch(CCmdUI* pCmdUI);
	afx_msg void OnSpinTransparencyPatch();
	afx_msg void OnUpdateSpinTransparencyPatch(CCmdUI* pCmdUI);
	afx_msg void OnSpinTransparencyline();
	afx_msg void OnUpdateSpinTransparencyline(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpinTransparencypicture(CCmdUI* pCmdUI);
	afx_msg void OnSpinTransparencypicture();
	afx_msg void OnSpinBlackregionthreshold();
	afx_msg void OnUpdateSpinBlackregionthreshold(CCmdUI* pCmdUI);
	afx_msg void OnSpinTransparencytriangleline();
	afx_msg void OnUpdateSpinTransparencytriangleline(CCmdUI* pCmdUI);
	afx_msg void OnSpinTransparencylineskeleton();
	afx_msg void OnUpdateSpinTransparencylineskeleton(CCmdUI* pCmdUI);
};


