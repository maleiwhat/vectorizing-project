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

// VAV_MainFrm.cpp : VAV_MainFrame ���O����@
//

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include "VAV_App.h"
#include "VAV_MainFrm.h"
#include "VAV_View.h"
#include "cvshowEX.h"
#include "ConvStr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// VAV_MainFrame

IMPLEMENT_DYNCREATE(VAV_MainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(VAV_MainFrame, CFrameWndEx)
	// 0
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7,
					 &VAV_MainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7,
							   &VAV_MainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &VAV_MainFrame::OnViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR,
						 &VAV_MainFrame::OnUpdateViewCaptionBar)
	ON_COMMAND(ID_TOOLS_OPTIONS, &VAV_MainFrame::OnOptions)
	ON_COMMAND(ID_FILE_PRINT, &VAV_MainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &VAV_MainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &VAV_MainFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW,
						 &VAV_MainFrame::OnUpdateFilePrintPreview)
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_FILE_OPEN_PICTURE, &VAV_MainFrame::OnFileOpenPicture)
	ON_COMMAND(ID_FILE_OPEN_VIDEO, &VAV_MainFrame::OnFileOpenVideo)
	ON_COMMAND(ID_FILE_OPEN_VIDEO2, &VAV_MainFrame::OnFileOpenVideo2)
	ON_COMMAND(ID_BUTTON_Canny, &VAV_MainFrame::OnButtonCanny)
	ON_COMMAND(ID_SPIN_CannyThreshold1, &VAV_MainFrame::OnSpinCannyThreshold1)
	ON_COMMAND(ID_SPIN_CannyThreshold2, &VAV_MainFrame::OnSpinCannyThreshold2)
	ON_COMMAND(ID_SPIN_ApertureSize, &VAV_MainFrame::OnSpinAperturesize)
	ON_COMMAND(ID_BUTTON_ControlPointInitialize,
			   &VAV_MainFrame::OnButtonControlPointInitialize)
	ON_COMMAND(ID_BUTTON_CGALVectorization,
			   &VAV_MainFrame::OnButtonCGALTriangulation)
	ON_COMMAND(ID_BUTTON_Skeleton, &VAV_MainFrame::OnButtonSkeleton)
	ON_COMMAND(ID_BUTTON_Sobel, &VAV_MainFrame::OnButtonSobel)
	ON_COMMAND(ID_BUTTON_Laplace, &VAV_MainFrame::OnButtonLaplace)
	// 1
	ON_COMMAND(ID_SPIN_TransparencySelectPatch,
			   &VAV_MainFrame::OnSpinTransparencySelectPatch)
	ON_UPDATE_COMMAND_UI(ID_SPIN_TransparencySelectPatch,
						 &VAV_MainFrame::OnUpdateSpinTransparencySelectPatch)
	ON_COMMAND(ID_SPIN_TransparencyPatch, &VAV_MainFrame::OnSpinTransparencyPatch)
	ON_UPDATE_COMMAND_UI(ID_SPIN_TransparencyPatch,
						 &VAV_MainFrame::OnUpdateSpinTransparencyPatch)
	ON_COMMAND(ID_SPIN_TransparencyLine, &VAV_MainFrame::OnSpinTransparencyline)
	ON_UPDATE_COMMAND_UI(ID_SPIN_TransparencyLine,
						 &VAV_MainFrame::OnUpdateSpinTransparencyline)
	ON_UPDATE_COMMAND_UI(ID_SPIN_TransparencyPicture,
						 &VAV_MainFrame::OnUpdateSpinTransparencypicture)
	ON_COMMAND(ID_SPIN_TransparencyPicture,
			   &VAV_MainFrame::OnSpinTransparencypicture)
	ON_COMMAND(ID_SPIN_BlackRegionThreshold,
			   &VAV_MainFrame::OnSpinBlackregionthreshold)
	ON_UPDATE_COMMAND_UI(ID_SPIN_BlackRegionThreshold,
						 &VAV_MainFrame::OnUpdateSpinBlackregionthreshold)
	ON_COMMAND(ID_SPIN_TransparencyTriangleLine,
			   &VAV_MainFrame::OnSpinTransparencytriangleline)
	ON_UPDATE_COMMAND_UI(ID_SPIN_TransparencyTriangleLine,
						 &VAV_MainFrame::OnUpdateSpinTransparencytriangleline)
	ON_COMMAND(ID_SPIN_TransparencyLineSkeleton,
			   &VAV_MainFrame::OnSpinTransparencylineskeleton)
	ON_UPDATE_COMMAND_UI(ID_SPIN_TransparencyLineSkeleton,
						 &VAV_MainFrame::OnUpdateSpinTransparencylineskeleton)
	ON_COMMAND(ID_CHECK_DRAW_PATCH, &VAV_MainFrame::OnCheckDrawPatch)
	ON_UPDATE_COMMAND_UI(ID_CHECK_DRAW_PATCH, &VAV_MainFrame::OnUpdateCheckDrawPatch)
	ON_COMMAND(ID_CHECK_DRAW_SEPARATE_PATCH, &VAV_MainFrame::OnCheckDrawSeparatePatch)
	ON_UPDATE_COMMAND_UI(ID_CHECK_DRAW_SEPARATE_PATCH, &VAV_MainFrame::OnUpdateCheckDrawSeparatePatch)
	ON_COMMAND(ID_CHECK_DRAW_CONTOUR, &VAV_MainFrame::OnCheckDrawContour)
	ON_UPDATE_COMMAND_UI(ID_CHECK_DRAW_CONTOUR, &VAV_MainFrame::OnUpdateCheckDrawContour)
	ON_COMMAND(ID_CHECK_DRAW_CONTOUR_CONTROL_POINT, &VAV_MainFrame::OnCheckDrawContourControlPoint)
	ON_UPDATE_COMMAND_UI(ID_CHECK_DRAW_CONTOUR_CONTROL_POINT,
						 &VAV_MainFrame::OnUpdateCheckDrawContourControlPoint)
	ON_COMMAND(ID_CHECK_DRAW_CONTOUR_SKELETON_POINT, &VAV_MainFrame::OnCheckDrawContourSkeletonPoint)
	ON_UPDATE_COMMAND_UI(ID_CHECK_DRAW_CONTOUR_SKELETON_POINT,
						 &VAV_MainFrame::OnUpdateCheckDrawContourSkeletonPoint)
	ON_COMMAND(ID_CHECK_DRAW_ISOSURFACE, &VAV_MainFrame::OnCheckDrawIsosurface)
	ON_UPDATE_COMMAND_UI(ID_CHECK_DRAW_ISOSURFACE, &VAV_MainFrame::OnUpdateCheckDrawIsosurface)
	ON_COMMAND(ID_CHECK_DRAW_CURVE_EXTRACTION, &VAV_MainFrame::OnCheckDrawCurveExtraction)
	ON_UPDATE_COMMAND_UI(ID_CHECK_DRAW_CURVE_EXTRACTION,
						 &VAV_MainFrame::OnUpdateCheckDrawCurveExtraction)
	ON_COMMAND(ID_CHECK_DRAW_CANNY_EXTRACTION, &VAV_MainFrame::OnCheckDrawCannyExtraction)
	ON_UPDATE_COMMAND_UI(ID_CHECK_DRAW_CANNY_EXTRACTION,
						 &VAV_MainFrame::OnUpdateCheckDrawCannyExtraction)
	ON_COMMAND(ID_CHECK_Constraint_Curves_parameter_1,
			   &VAV_MainFrame::OnCheck_ConstraintCurvesParameter1)
	ON_UPDATE_COMMAND_UI(ID_CHECK_Constraint_Curves_parameter_1,
						 &VAV_MainFrame::OnUpdateCheck_ConstraintCurvesParameter1)
	ON_COMMAND(ID_CHECK_Constraint_Curves_parameter_2,
			   &VAV_MainFrame::OnCheck_ConstraintCurvesParameter2)
	ON_UPDATE_COMMAND_UI(ID_CHECK_Constraint_Curves_parameter_2,
						 &VAV_MainFrame::OnUpdateCheck_ConstraintCurvesParameter2)
	ON_COMMAND(ID_CHECK_Isosurface_Constraint, &VAV_MainFrame::OnCheck_IsosurfaceConstraint)
	ON_UPDATE_COMMAND_UI(ID_CHECK_Isosurface_Constraint,
						 &VAV_MainFrame::OnUpdateCheck_IsosurfaceConstraint)
	ON_COMMAND(ID_CHECK_Decorative_Curves, &VAV_MainFrame::OnCheck_DecorativeCurves)
	ON_UPDATE_COMMAND_UI(ID_CHECK_Decorative_Curves, &VAV_MainFrame::OnUpdateCheck_DecorativeCurves)
	ON_COMMAND(ID_CHECK_Boundary_Curves, &VAV_MainFrame::OnCheck_BoundaryCurves)
	ON_UPDATE_COMMAND_UI(ID_CHECK_Boundary_Curves, &VAV_MainFrame::OnUpdateCheck_BoundaryCurves)
	ON_COMMAND(ID_CHECK_Region_Growing, &VAV_MainFrame::OnCheck_RegionGrowing)
	ON_UPDATE_COMMAND_UI(ID_CHECK_Region_Growing, &VAV_MainFrame::OnUpdateCheck_RegionGrowing)
	ON_COMMAND(ID_CHECK_Black_Line_Vectorization, &VAV_MainFrame::OnCheck_BlackLineVectorization)
	ON_UPDATE_COMMAND_UI(ID_CHECK_Black_Line_Vectorization,
						 &VAV_MainFrame::OnUpdateCheck_BlackLineVectorization)
	ON_COMMAND(ID_CHECK_Isosurface_Constraint_Vectorization,
			   &VAV_MainFrame::OnCheck_IsosurfaceConstraintVectorization)
	ON_UPDATE_COMMAND_UI(ID_CHECK_Isosurface_Constraint_Vectorization,
						 &VAV_MainFrame::OnUpdateCheck_IsosurfaceConstraintVectorization)
	ON_COMMAND(ID_BUTTON_Build_Vectorization, &VAV_MainFrame::OnButton_BuildVectorization)
	ON_COMMAND(ID_CHECK_Decorative_Curves_Constraint,
			   &VAV_MainFrame::OnCheck_DecorativeCurvesConstraint)
	ON_UPDATE_COMMAND_UI(ID_CHECK_Decorative_Curves_Constraint,
						 &VAV_MainFrame::OnUpdateCheck_DecorativeCurvesConstraint)
	ON_COMMAND(ID_CHECK_Boundary_Curves_Constraint, &VAV_MainFrame::OnCheck_BoundaryCurvesConstraint)
	ON_UPDATE_COMMAND_UI(ID_CHECK_Boundary_Curves_Constraint,
						 &VAV_MainFrame::OnUpdateCheck_BoundaryCurvesConstraint)
	ON_COMMAND(ID_CHECK_Boundary_Curves_Vectorization,
			   &VAV_MainFrame::OnCheck_BoundaryCurvesVectorization)
	ON_UPDATE_COMMAND_UI(ID_CHECK_Boundary_Curves_Vectorization,
						 &VAV_MainFrame::OnUpdateCheck_BoundaryCurvesVectorization)
	ON_COMMAND(ID_CHECK_Isosurface_Region, &VAV_MainFrame::OnCheck_IsosurfaceRegion)
	ON_UPDATE_COMMAND_UI(ID_CHECK_Isosurface_Region, &VAV_MainFrame::OnUpdateCheck_IsosurfaceRegion)
	ON_COMMAND(ID_BUTTON_Curve_matching, &VAV_MainFrame::OnButton_Curvematching)
	ON_COMMAND(ID_BUTTON_Zhang09, &VAV_MainFrame::OnButton_Zhang09)
END_MESSAGE_MAP()

// VAV_MainFrame �غc/�Ѻc
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

VAV_MainFrame::VAV_MainFrame()
{
	// TODO: �b���[�J������l�Ƶ{���X
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"),
									  ID_VIEW_APPLOOK_OFF_2007_BLACK);
	AllocConsole();
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;
	// init draw setting
	m_DRAW_PATCH = false;
	m_DRAW_SEPARATE_PATCH = true;
	m_DRAW_CONTOUR = false;
	m_DRAW_CONTOUR_CONTROL_POINT = false;
	m_DRAW_CONTOUR_SKELETON_POINT = false;
	m_DRAW_ISOSURFACE = false;
	m_DRAW_CURVE_EXTRACTION = false;
	m_DRAW_CANNY_EXTRACTION = false;
	m_CONSTRAINT_CURVES_PARAMETER_1 = true;
	m_CONSTRAINT_CURVES_PARAMETER_2 = true;
	m_ISOSURFACE_REGION = true;
	m_ISOSURFACE_CONSTRAINT = false;
	m_ISOSURFACE_CONSTRAINT_VECTORIZATION = true;
	m_DECORATIVE_CURVES = true;
	m_DECORATIVE_CURVES_CONSTRAINT = false;
	m_BOUNDARY_CURVES = true;
	m_BOUNDARY_CURVES_CONSTRAINT = false;
	m_REGION_GROWING = true;
	m_BLACK_LINE_VECTORIZATION = true;
	m_BOUNDARY_CURVES_VECTORIZATION = true;
	static int iinit = 0;
	if(!iinit)
	{
		iinit = 1;
		g_cvshowEX.Init();
	}
}

VAV_MainFrame::~VAV_MainFrame()
{
}

int VAV_MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CFrameWndEx::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	BOOL bNameValid;
	// �ھګ���ȳ]�w��ı�ƺ޲z���M�˦�
	OnApplicationLook(theApp.m_nAppLook);
	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);
	if(!m_wndStatusBar.Create(this))
	{
		TRACE0("�L�k�إߪ��A�C\n");
		return -1;      // �L�k�إ�
	}
	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1,
							  strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(
										  ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);
	// �ҥ� Visual Studio 2005 �˦����n�����欰
	CDockingManager::SetDockingMode(DT_SMART);
	// �ҥ� Visual Studio 2005 �˦����n�����۰����æ欰
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	// �إ߼��D�C:
	if(!CreateCaptionBar())
	{
		TRACE0("�L�k�إ߼��D�C\n");
		return -1;      // �L�k�إ�
	}
	// ���J�\����ؼv�� (����b����зǤu��C�W):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES,
			theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);
	// �إ߰��n����
	if(!CreateDockingWindows())
	{
		TRACE0("�L�k�إ߰��n����\n");
		return -1;
	}
	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndFileView);
	CDockablePane* pTabbedBar = NULL;
	m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);
	return 0;
}

BOOL VAV_MainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CFrameWndEx::PreCreateWindow(cs))
	{
		return FALSE;
	}
	// TODO: �b���g�ѭק� CREATESTRUCT cs
	// �F��ק�������O�μ˦����ت�
	return TRUE;
}

BOOL VAV_MainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	// �إ����O�˵�
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	if(!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE,
							  ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
							  CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("�L�k�إ� [���O�˵�] ����\n");
		return FALSE; // �L�k�إ�
	}
	// �إ��ɮ��˵�
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if(!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE,
							 ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
							 CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("�L�k�إ� [�ɮ��˵�] ����\n");
		return FALSE; // �L�k�إ�
	}
	// �إ߿�X����
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if(!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE,
						   ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
						   CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("�L�k�إ� [��X] ����\n");
		return FALSE; // �L�k�إ�
	}
	// �إ��ݩʵ���
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if(!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE,
							   ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
							   | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("�L�k�إ� [�ݩ�] ����\n");
		return FALSE; // �L�k�إ�
	}
	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void VAV_MainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(),
						  MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON,
						  ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);
	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(),
						   MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON,
						   ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndClassView.SetIcon(hClassViewIcon, FALSE);
	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(),
						   MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON,
						   ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);
	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(),
							   MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND),
							   IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON),
							   0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);
}

BOOL VAV_MainFrame::CreateCaptionBar()
{
	if(!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this,
							   ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("�L�k�إ߼��D�C\n");
		return FALSE;
	}
	BOOL bNameValid;
	CString strTemp, strTemp2;
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT,
							  FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);
	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);
	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE,
							  CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);
	return TRUE;
}

// VAV_MainFrame �E�_

#ifdef _DEBUG
void VAV_MainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void VAV_MainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// VAV_MainFrame �T���B�z�`��

void VAV_MainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;
	theApp.m_nAppLook = id;
	switch(theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;
	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;
	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;
	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(
				CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;
	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;
	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;
	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;
	default:
		switch(theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(
				CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;
		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(
				CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;
		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(
				CMFCVisualManagerOffice2007::Office2007_Silver);
			break;
		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(
				CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(
				CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}
	RedrawWindow(NULL, NULL,
				 RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);
	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void VAV_MainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void VAV_MainFrame::OnViewCaptionBar()
{
	m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
}

void VAV_MainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

void VAV_MainFrame::OnOptions()
{
	CMFCRibbonCustomizeDialog* pOptionsDlg = new CMFCRibbonCustomizeDialog(this,
			&m_wndRibbonBar);
	ASSERT(pOptionsDlg != NULL);
	pOptionsDlg->DoModal();
	delete pOptionsDlg;
}

void VAV_MainFrame::OnFilePrint()
{
	if(IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void VAV_MainFrame::OnFilePrintPreview()
{
	if(IsPrintPreview())
	{
		PostMessage(WM_COMMAND,
					AFX_ID_PREVIEW_CLOSE);   // �j��w���C�L�Ҧ�����
	}
}

void VAV_MainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsPrintPreview());
}

void VAV_MainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}


void VAV_MainFrame::OnSpinCannyThreshold1()
{
}


void VAV_MainFrame::OnSpinCannyThreshold2()
{
}


void VAV_MainFrame::OnSpinAperturesize()
{
}

void VAV_MainFrame::OnButtonControlPointInitialize()
{
}

void VAV_MainFrame::OnButtonSobel()
{
	m_cannyImage = m_vavImage;
	GetVavView()->SetTexture(m_cannyImage.GetDx11Texture());
}
void VAV_MainFrame::OnButtonLaplace()
{
}
void VAV_MainFrame::OnSpinTransparencySelectPatch()
{
	GetVavView()->
	GetD3DApp().SetTransparency_SelectPatch((100 - m_SelectPatchTransparency) *
											0.01);
}

void VAV_MainFrame::OnUpdateSpinTransparencySelectPatch(CCmdUI* pCmdUI)
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = 0;
	for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
	{
		tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
					 ID_SPIN_TransparencySelectPatch);
		if(tmp_ui != 0)
		{
			break;
		}
	}
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
	if(NULL != re)
	{
		m_SelectPatchTransparency = atoi(ConvStr::GetStr(
											 re->GetEditText().GetString()).c_str());
	}
	//GetVavView()->GetD3DApp().SetTransparency_SelectPatch((100 - m_SelectPatchTransparency) *	0.01);
}
void VAV_MainFrame::ShowPatch(double x, double y)
{
// 	GetVavView()->GetD3DApp().ClearPatchs();
// 	for(int i = 0; i < m_CvPatchs.size(); ++i)
// 	{
//      if (m_CvPatchs[i].Inside(x, y))
//      {
//          TriangulationCgal_Patch cgal_patch;
//          Patch patch = ToPatch(m_CvPatchs[i]);
//          patch.SplinePoints(0.3);
//          cgal_patch.AddPatch(patch);
//          cgal_patch.SetCriteria(0.125, 40);
//          cgal_patch.Compute();
//          GetD3DApp().AddPatchTriangles(cgal_patch.GetTriangles(), m_vavImage.GetHeight());
//      }
//	}
}

void VAV_MainFrame::OnSpinTransparencyPatch()
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = 0;
	for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
	{
		tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
					 ID_SPIN_TransparencyPatch);
		if(tmp_ui != 0)
		{
			break;
		}
	}
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
	if(NULL != re)
	{
		m_PatchTransparency = atoi(ConvStr::GetStr(
									   re->GetEditText().GetString()).c_str());
	}
	GetVavView()->GetD3DApp().SetTransparency_Triangle((
				100 - m_PatchTransparency) * 0.01);
}
void VAV_MainFrame::OnUpdateSpinTransparencyPatch(CCmdUI* pCmdUI)
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = 0;
	for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
	{
		tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
					 ID_SPIN_TransparencyPatch);
		if(tmp_ui != 0)
		{
			break;
		}
	}
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
	if(NULL != re)
	{
		m_PatchTransparency = atoi(ConvStr::GetStr(
									   re->GetEditText().GetString()).c_str());
	}
	GetVavView()->GetD3DApp().SetTransparency_Triangle((
				100 - m_PatchTransparency) * 0.01);
}
void VAV_MainFrame::OnSpinTransparencyline()
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = 0;
	for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
	{
		tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
					 ID_SPIN_TransparencyLine);
		if(tmp_ui != 0)
		{
			break;
		}
	}
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
	if(NULL != re)
	{
		m_LineTransparency = atoi(ConvStr::GetStr(
									  re->GetEditText().GetString()).c_str());
	}
	GetVavView()->GetD3DApp().SetTransparency_Line((
				100 - m_LineTransparency) * 0.01);
}
void VAV_MainFrame::OnUpdateSpinTransparencyline(CCmdUI* pCmdUI)
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = 0;
	for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
	{
		tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
					 ID_SPIN_TransparencyLine);
		if(tmp_ui != 0)
		{
			break;
		}
	}
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
	if(NULL != re)
	{
		m_LineTransparency = atoi(ConvStr::GetStr(
									  re->GetEditText().GetString()).c_str());
	}
	GetVavView()->GetD3DApp().SetTransparency_Line((
				100 - m_LineTransparency) * 0.01);
}
void VAV_MainFrame::OnUpdateSpinTransparencypicture(CCmdUI* pCmdUI)
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = 0;
	for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
	{
		tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
					 ID_SPIN_TransparencyPicture);
		if(tmp_ui != 0)
		{
			break;
		}
	}
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
	if(NULL != re)
	{
		m_PictureTransparency = atoi(ConvStr::GetStr(
										 re->GetEditText().GetString()).c_str());
	}
	GetVavView()->GetD3DApp().SetTransparency_Picture((
				100 - m_PictureTransparency) * 0.01);
}
void VAV_MainFrame::OnSpinTransparencypicture()
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = 0;
	for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
	{
		tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
					 ID_SPIN_TransparencyPicture);
		if(tmp_ui != 0)
		{
			break;
		}
	}
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
	if(NULL != re)
	{
		m_PictureTransparency = atoi(ConvStr::GetStr(
										 re->GetEditText().GetString()).c_str());
	}
	GetVavView()->GetD3DApp().SetTransparency_Picture((
				100 - m_PictureTransparency) * 0.01);
}
void VAV_MainFrame::OnSpinBlackregionthreshold()
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = 0;
	for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
	{
		tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
					 ID_SPIN_BlackRegionThreshold);
		if(tmp_ui != 0)
		{
			break;
		}
	}
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
	if(NULL != re)
	{
		m_BlackRegionThreshold = atoi(ConvStr::GetStr(
										  re->GetEditText().GetString()).c_str());
	}
}
void VAV_MainFrame::OnUpdateSpinBlackregionthreshold(CCmdUI* pCmdUI)
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = 0;
	for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
	{
		tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
					 ID_SPIN_BlackRegionThreshold);
		if(tmp_ui != 0)
		{
			break;
		}
	}
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
	if(NULL != re)
	{
		m_BlackRegionThreshold = atoi(ConvStr::GetStr(
										  re->GetEditText().GetString()).c_str());
	}
}
void VAV_MainFrame::OnSpinTransparencytriangleline()
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = 0;
	for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
	{
		tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
					 ID_SPIN_TransparencyTriangleLine);
		if(tmp_ui != 0)
		{
			break;
		}
	}
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
	if(NULL != re)
	{
		m_TriangleLineTransparency = atoi(ConvStr::GetStr(
											  re->GetEditText().GetString()).c_str());
	}
	GetVavView()->GetD3DApp().SetTransparency_TriangleLine((
				100 - m_TriangleLineTransparency) * 0.01);
}
void VAV_MainFrame::OnUpdateSpinTransparencytriangleline(CCmdUI* pCmdUI)
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = 0;
	for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
	{
		tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
					 ID_SPIN_TransparencyTriangleLine);
		if(tmp_ui != 0)
		{
			break;
		}
	}
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
	if(NULL != re)
	{
		m_TriangleLineTransparency = atoi(ConvStr::GetStr(
											  re->GetEditText().GetString()).c_str());
	}
	GetVavView()->GetD3DApp().SetTransparency_TriangleLine((
				100 - m_TriangleLineTransparency) * 0.01);
}
void VAV_MainFrame::OnSpinTransparencylineskeleton()
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = 0;
	for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
	{
		tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
					 ID_SPIN_TransparencyLineSkeleton);
		if(tmp_ui != 0)
		{
			break;
		}
	}
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
	if(NULL != re)
	{
		m_LineSkeletonTransparency = atoi(ConvStr::GetStr(
											  re->GetEditText().GetString()).c_str());
	}
	GetVavView()->GetD3DApp().SetTransparency_LineSkeleton((
				100 - m_LineSkeletonTransparency) * 0.01);
}
void VAV_MainFrame::OnUpdateSpinTransparencylineskeleton(CCmdUI* pCmdUI)
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = 0;
	for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
	{
		tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
					 ID_SPIN_TransparencyLineSkeleton);
		if(tmp_ui != 0)
		{
			break;
		}
	}
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
	if(NULL != re)
	{
		m_LineSkeletonTransparency = atoi(ConvStr::GetStr(
											  re->GetEditText().GetString()).c_str());
	}
	GetVavView()->GetD3DApp().SetTransparency_LineSkeleton((
				100 - m_LineSkeletonTransparency) * 0.01);
}

void VAV_MainFrame::OnCheckDrawPatch()
{
	m_DRAW_PATCH = !m_DRAW_PATCH;
}


void VAV_MainFrame::OnUpdateCheckDrawPatch(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_DRAW_PATCH);
}


void VAV_MainFrame::OnCheckDrawSeparatePatch()
{
	m_DRAW_SEPARATE_PATCH = !m_DRAW_SEPARATE_PATCH;
}


void VAV_MainFrame::OnUpdateCheckDrawSeparatePatch(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_DRAW_SEPARATE_PATCH);
}


void VAV_MainFrame::OnCheckDrawContour()
{
	m_DRAW_CONTOUR = !m_DRAW_CONTOUR;
}


void VAV_MainFrame::OnUpdateCheckDrawContour(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_DRAW_CONTOUR);
}


void VAV_MainFrame::OnCheckDrawContourControlPoint()
{
	m_DRAW_CONTOUR_CONTROL_POINT = !m_DRAW_CONTOUR_CONTROL_POINT;
}


void VAV_MainFrame::OnUpdateCheckDrawContourControlPoint(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_DRAW_CONTOUR_CONTROL_POINT);
}


void VAV_MainFrame::OnCheckDrawContourSkeletonPoint()
{
	m_DRAW_CONTOUR_SKELETON_POINT = !m_DRAW_CONTOUR_SKELETON_POINT;
}


void VAV_MainFrame::OnUpdateCheckDrawContourSkeletonPoint(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_DRAW_CONTOUR_SKELETON_POINT);
}


void VAV_MainFrame::OnCheckDrawIsosurface()
{
	m_DRAW_ISOSURFACE = !m_DRAW_ISOSURFACE;
}


void VAV_MainFrame::OnUpdateCheckDrawIsosurface(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_DRAW_ISOSURFACE);
}


void VAV_MainFrame::OnCheckDrawCurveExtraction()
{
	m_DRAW_CURVE_EXTRACTION = !m_DRAW_CURVE_EXTRACTION;
}


void VAV_MainFrame::OnUpdateCheckDrawCurveExtraction(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_DRAW_CURVE_EXTRACTION);
}


void VAV_MainFrame::OnCheckDrawCannyExtraction()
{
	m_DRAW_CANNY_EXTRACTION = !m_DRAW_CANNY_EXTRACTION;
}


void VAV_MainFrame::OnUpdateCheckDrawCannyExtraction(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_DRAW_CANNY_EXTRACTION);
}

void VAV_MainFrame::OnCheck_ConstraintCurvesParameter1()
{
	m_CONSTRAINT_CURVES_PARAMETER_1 = !m_CONSTRAINT_CURVES_PARAMETER_1;
}


void VAV_MainFrame::OnUpdateCheck_ConstraintCurvesParameter1(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_CONSTRAINT_CURVES_PARAMETER_1);
}


void VAV_MainFrame::OnCheck_ConstraintCurvesParameter2()
{
	m_CONSTRAINT_CURVES_PARAMETER_2 = !m_CONSTRAINT_CURVES_PARAMETER_2;
}


void VAV_MainFrame::OnUpdateCheck_ConstraintCurvesParameter2(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_CONSTRAINT_CURVES_PARAMETER_2);
}


void VAV_MainFrame::OnCheck_IsosurfaceConstraint()
{
	m_ISOSURFACE_CONSTRAINT = !m_ISOSURFACE_CONSTRAINT;
}


void VAV_MainFrame::OnUpdateCheck_IsosurfaceConstraint(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_ISOSURFACE_CONSTRAINT);
}


void VAV_MainFrame::OnCheck_DecorativeCurves()
{
	m_DECORATIVE_CURVES = !m_DECORATIVE_CURVES;
}


void VAV_MainFrame::OnUpdateCheck_DecorativeCurves(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_DECORATIVE_CURVES);
}


void VAV_MainFrame::OnCheck_BoundaryCurves()
{
	m_BOUNDARY_CURVES = !m_BOUNDARY_CURVES;
}


void VAV_MainFrame::OnUpdateCheck_BoundaryCurves(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_BOUNDARY_CURVES);
}


void VAV_MainFrame::OnCheck_RegionGrowing()
{
	m_REGION_GROWING = !m_REGION_GROWING;
}


void VAV_MainFrame::OnUpdateCheck_RegionGrowing(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_REGION_GROWING);
}


void VAV_MainFrame::OnCheck_BlackLineVectorization()
{
	m_BLACK_LINE_VECTORIZATION = !m_BLACK_LINE_VECTORIZATION;
}


void VAV_MainFrame::OnUpdateCheck_BlackLineVectorization(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_BLACK_LINE_VECTORIZATION);
}


void VAV_MainFrame::OnCheck_IsosurfaceConstraintVectorization()
{
	m_ISOSURFACE_CONSTRAINT_VECTORIZATION = !m_ISOSURFACE_CONSTRAINT_VECTORIZATION;
}


void VAV_MainFrame::OnUpdateCheck_IsosurfaceConstraintVectorization(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_ISOSURFACE_CONSTRAINT_VECTORIZATION);
}

VAV_View* VAV_MainFrame::GetVavView()
{
	return ((VAV_View*)this->GetActiveView());
}


void VAV_MainFrame::OnCheck_DecorativeCurvesConstraint()
{
	m_DECORATIVE_CURVES_CONSTRAINT = !m_DECORATIVE_CURVES_CONSTRAINT;
}


void VAV_MainFrame::OnUpdateCheck_DecorativeCurvesConstraint(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_DECORATIVE_CURVES_CONSTRAINT);
}


void VAV_MainFrame::OnCheck_BoundaryCurvesConstraint()
{
	m_BOUNDARY_CURVES_CONSTRAINT = !m_BOUNDARY_CURVES_CONSTRAINT;
}


void VAV_MainFrame::OnUpdateCheck_BoundaryCurvesConstraint(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_BOUNDARY_CURVES_CONSTRAINT);
}


void VAV_MainFrame::OnCheck_BoundaryCurvesVectorization()
{
	m_BOUNDARY_CURVES_VECTORIZATION = !m_BOUNDARY_CURVES_VECTORIZATION;
}


void VAV_MainFrame::OnUpdateCheck_BoundaryCurvesVectorization(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_BOUNDARY_CURVES_VECTORIZATION);
}


void VAV_MainFrame::OnCheck_IsosurfaceRegion()
{
	m_ISOSURFACE_REGION = !m_ISOSURFACE_REGION;
}


void VAV_MainFrame::OnUpdateCheck_IsosurfaceRegion(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_ISOSURFACE_REGION);
}
