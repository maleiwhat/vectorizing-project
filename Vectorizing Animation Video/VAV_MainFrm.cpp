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
#include "ConvStr.h"
#include "TriangulationCgal_Sideline.h"
#include "TriangulationCgal_Patch.h"
#include "VoronoiCgal_Patch.h"
#include "CvExtenstion.h"
#include "CvExtenstion2.h"
#include "IFExtenstion.h"
#include "CmCurveEx.h"
#include "Line.h"
#include "CvExtenstion0.h"
#include "zip/ZipFolder.h"

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
	ON_COMMAND(ID_COMBO_ColorInterpolation,
			   &VAV_MainFrame::OnComboColorinterpolation)
	ON_COMMAND(ID_FILE_OPEN_PICTURE, &VAV_MainFrame::OnFileOpenPicture)
	ON_COMMAND(ID_FILE_OPEN_VIDEO, &VAV_MainFrame::OnFileOpenVideo)
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


void VAV_MainFrame::OnComboColorinterpolation()
{
}

void VAV_MainFrame::OnFileOpenPicture()
{
	CFileDialog dlg(TRUE);
	dlg.m_ofn.lpstrFilter   = L"All Files (*.*)\0*.*\0\0";
	dlg.m_ofn.lpstrTitle    = L"Load File";
	CString filename;
	if(dlg.DoModal() == IDOK)
	{
		filename = dlg.GetPathName(); // return full path and filename
		if(filename.GetLength() > 1)
		{
			D3DApp& d3dApp = GetVavView()->GetD3DApp();
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
			d3dApp.SetTransparency_SelectPatch((100 - m_SelectPatchTransparency) * 0.01);
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
			d3dApp.SetTransparency_SelectPatch((100 - m_PatchTransparency) * 0.01);
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
			d3dApp.SetTransparency_SelectPatch((100 - m_TriangleLineTransparency) * 0.01);
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
			d3dApp.SetTransparency_Picture((100 - m_PictureTransparency) * 0.01);
			m_vavImage.ReadImage(ConvStr::GetStr(filename.GetString()));
			d3dApp.ClearTriangles();
			GetVavView()->SetPictureSize(m_vavImage.GetWidth(), m_vavImage.GetHeight());
			GetVavView()->SetTexture(m_vavImage.GetDx11Texture());
		}
	}
}

void VAV_MainFrame::OnButtonCanny()
{
	if(!m_vavImage.Vaild())
	{
		return;
	}
	int t1 = 0, t2 = 30, a = 3;
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = 0;
	for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
	{
		tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
					 ID_SPIN_CannyThreshold1);
		if(tmp_ui != 0)
		{
			break;
		}
	}
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
	if(NULL != re)
	{
		t1 = atoi(ConvStr::GetStr(re->GetEditText().GetString()).c_str());
	}
	for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
	{
		tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
					 ID_SPIN_CannyThreshold2);
		if(tmp_ui != 0)
		{
			break;
		}
	}
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
	if(NULL != re)
	{
		t2 = atoi(ConvStr::GetStr(re->GetEditText().GetString()).c_str());
	}
	for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
	{
		tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
					 ID_SPIN_ApertureSize);
		if(tmp_ui != 0)
		{
			break;
		}
	}
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
	if(NULL != re)
	{
		a = atoi(ConvStr::GetStr(re->GetEditText().GetString()).c_str());
	}
	m_cannyImage = m_vavImage;
	cv::Mat ce = CannyEdge(m_vavImage, t1, t2, a);
//  Lines lines1 = ComputeEdgeLine(ce);
//  Lines lines = m_vavImage.AnimaEdge(5, 0.01, 0.1);
//  lines = ComputeTrappedBallEdge(m_vavImage, lines, 5);
//  cv::Mat lineImage = MakeLineImage(m_vavImage, lines);
//  lines = GetLines(GetSidelines(lineImage));
//  m_cannyImage.ShowEdgeLine(lines);
	m_cannyImage = ce;
//  Skeleton(m_cannyImage);
//  cv::Mat cw, cw2;
//  cw2 = m_cannyImage;
//  cw2.convertTo(cw, CV_32FC3);
//  Collect_Water(cw, cw, 5, 5, m_BlackRegionThreshold * 0.01);
//  m_cannyImage = cw;
	GetVavView()->SetTexture(m_cannyImage.GetDx11Texture());
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

void VAV_MainFrame::OnButtonSkeleton()
{
	m_cannyImage = m_vavImage;
	//Skeleton(m_cannyImage);
	cv::Mat cw = cv::Mat(m_cannyImage).clone();
	for(int i = 0; i < cw.rows; i++)
	{
		for(int j = 0; j < cw.cols; j++)
		{
			cv::Vec3b& v = cw.at<cv::Vec3b>(i, j);
			v[0] = 255;
			v[1] = 255;
			v[2] = 255;
		}
	}
	m_cannyImage = cw;
	GetVavView()->SetTexture(m_cannyImage.GetDx11Texture());
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
	GetVavView()->
	GetD3DApp().SetTransparency_SelectPatch((100 - m_SelectPatchTransparency) *
											0.01);
}
void VAV_MainFrame::ShowPatch(double x, double y)
{
	GetVavView()->GetD3DApp().ClearPatchs();
	for(int i = 0; i < m_CvPatchs.size(); ++i)
	{
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
	}
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

/*
void VAV_MainFrame::AddFrame(cv::Mat img)
{
    D3DApp& d3dApp = GetVavView()->GetD3DApp();
    d3dApp.SetPictureSize(img.cols, img.rows);
    Lines decorativeLine;
    vavImage expImg(img);
    expImg.ToExpImage();
    const int MASK1_SIZE = 5;
    const int MASK2_SIZE = 5;
    const float secDer = 0.001f;
    cv::Mat ccp1 = expImg.Clone(), ccp2 = expImg.Clone();
    CmCurveEx* ccp1_curve = NULL;
    CmCurveEx* ccp2_curve = NULL;
    cvtColor(ccp1, ccp1, CV_BGR2GRAY);
    ccp1.convertTo(ccp1, CV_32F, 1.0 / 255);
    ccp1_curve = new CmCurveEx(ccp1);
    ccp1_curve->CalSecDer(MASK1_SIZE, secDer);
    ccp1_curve->Link();
    cvtColor(ccp2, ccp2, CV_BGR2GRAY);
    ccp2.convertTo(ccp2, CV_32F, 1.0 / 255);
    ccp2_curve = new CmCurveEx(ccp2);
    ccp2_curve->CalSecDer2(MASK2_SIZE, secDer);
    ccp2_curve->Link();
    CvLines tpnts2d;
    const CEdges& edges = ccp1_curve->GetEdges();
    for (size_t i = 0; i < edges.size(); i++)
    {
        tpnts2d.push_back(edges[i].pnts);
    }
    m_BlackLine = GetLines(tpnts2d, 0.5, 0.5);
    m_BlackLine = SmoothingLen5(m_BlackLine);
    Lines showLines;
    Lines BLineWidth(m_BlackLine.size());
    Lines normals = GetNormalsLen2(m_BlackLine);
    const double blackRadio = 0.5;
    for (int idx1 = 0; idx1 < m_BlackLine.size(); ++idx1)
    {
        const Line& nowLine = m_BlackLine[idx1];
        const Line& nowNormals = normals[idx1];
        Line& lineWidths = BLineWidth[idx1];
        lineWidths.clear();
        for (int idx2 = 0; idx2 < nowLine.size() - 1; ++idx2)
        {
            const double LINE_WIDTH = 5;
            Vector2 start(nowLine[idx2] - nowNormals[idx2] * LINE_WIDTH);
            Vector2 end(nowLine[idx2] + nowNormals[idx2] * LINE_WIDTH);
            Vector2 start2(nowLine[idx2 + 1] - nowNormals[idx2 + 1] * LINE_WIDTH);
            Vector2 end2(nowLine[idx2 + 1] + nowNormals[idx2 + 1] * LINE_WIDTH);
            double_vector line1 = expImg.GetLineLight(start.x, start.y, end.x, end.y,
                                  360);
            double_vector line2 = expImg.GetLineLight(start2.x, start2.y, end2.x, end2.y,
                                  360);
            double_vector width1 = GetLineWidth(ConvertToSquareWave(ConvertToAngle(line1),
                                                15, 50), LINE_WIDTH * 2);
            double_vector width2 = GetLineWidth(ConvertToSquareWave(ConvertToAngle(line2),
                                                15, 50), LINE_WIDTH * 2);
            if (width1.size() >= 2 && width2.size() >= 2 && abs(width2[0] - width2[1]) < 1.5)
            {
                Line line1;
                line1.push_back(nowLine[idx2] - nowNormals[idx2] * width1[0] * blackRadio);
                line1.push_back(nowLine[idx2 + 1] - nowNormals[idx2 + 1] * width2[0] *
                                blackRadio);
                line1 = GetLine(line1, 0.5, 0.5);
                Line line2;
                line2.push_back(nowLine[idx2] + nowNormals[idx2] * width1[1] * blackRadio);
                line2.push_back(nowLine[idx2 + 1] + nowNormals[idx2 + 1] * width2[1] *
                                blackRadio);
                line2 = GetLine(line2, 0.5, 0.5);
                showLines.push_back(line1);
                showLines.push_back(line2);
                // save line width
                lineWidths.push_back(Vector2(width1[0] * blackRadio, width1[1] * blackRadio));
            }
            else
            {
                lineWidths.push_back(Vector2());
            }
        }
        lineWidths.push_back(Vector2());
    }
    m_BLineWidth = BLineWidth;
    m_BlackLine = GetLines(tpnts2d, 0.5, 0.5);
    m_BlackLine = SmoothingLen5(m_BlackLine, 0.9, 5);
    LineEnds les = GetLineEnds(m_BlackLine);
    LinkLineEnds180(les, 5, 15);
    ConnectLineEnds(les, m_BlackLine, m_BLineWidth);
    IncreaseDensity(m_BlackLine, m_BLineWidth);
    les = GetLineEnds(m_BlackLine);
    // �s�������������u
    ConnectNearestLines(les, m_BlackLine, m_BLineWidth, 5, 15);
    m_BLineWidth = CleanOrphanedLineWidths(m_BLineWidth, 5);
    m_BLineWidth = FixLineWidths(m_BLineWidth, 50);
    //m_BLineWidth = FixLineWidths(m_BLineWidth, 100);
    ClearLineWidthByPercent(m_BLineWidth, 0.6);
    m_BLineWidth = FixLineWidths(m_BLineWidth, 200);
    m_BLineWidth = SmoothingHas0Len5(m_BLineWidth, 0, 5);
    m_BLineWidth = FixedLineWidth(m_BLineWidth, 5);
    Vector3s2d lineColors;
    lineColors = GetLinesColor(img, m_BlackLine);
    lineColors = SmoothingLen5(lineColors, 0, 10);
    m_BLineWidth = SmoothingLen5(m_BLineWidth, 0, 10);
    m_BlackLine = SmoothingLen5(m_BlackLine, 0, 15);
    d3dApp.AddLinesWidth(m_BlackLine, m_BLineWidth, lineColors);
    cv::Mat ccp2_simg;
    tpnts2d.clear();
    const CEdges& edges2 = ccp2_curve->GetEdges();
    for (size_t i = 0; i < edges2.size(); i++)
    {
        tpnts2d.push_back(edges2[i].pnts);
    }
    m_BlackLine2 = GetLines(tpnts2d, 0.5, 0.5);
    Lines tmp_width = m_BlackLine2;
    m_BlackLine2 = SmoothingLen5(m_BlackLine2, 0.5, 5);
    les = GetLineEnds(m_BlackLine2);
    LinkLineEnds180(les, 5, 60);
    ConnectLineEnds3(les, m_BlackLine2, tmp_width);
    les = GetLineEnds(m_BlackLine2);
    LinkLineEnds180(les, 8, 60);
    ConnectLineEnds3(les, m_BlackLine2, tmp_width);
    IncreaseDensity(m_BlackLine2, tmp_width);
    les = GetLineEnds(m_BlackLine2);
    ConnectNearestLines(les, m_BlackLine2, tmp_width, 8, 20);
    m_BlackLine2 = LineSplitAtEndIntersection(m_BlackLine2, 1);
    tmp_width = m_BlackLine2;
    les = GetLineEnds(m_BlackLine2);
    LinkLineEnds(les, 3, 360);
    ConnectLineEnds4(les, m_BlackLine2, tmp_width);
    les = GetLineEnds(m_BlackLine2);
    LinkLineEnds(les, 3, 360);
    ConnectLineEnds4(les, m_BlackLine2, tmp_width);
    m_BlackLine2 = SmoothingLen5(m_BlackLine2, 0.5, 8);
    d3dApp.AddLines(m_BlackLine2);
    ColorConstraints ccms;
    ColorConstraints ccms2;
    cv::Mat cmmsIndexImg;
    d3dApp.SetScaleTemporary(2);
    d3dApp.BuildPoint();
    d3dApp.InterSetRenderTransparencyOutput2();
    cv::Mat simg = d3dApp.DrawSceneToCvMat();
    d3dApp.SetScaleRecovery();
    cmmsIndexImg = simg.clone();
    cmmsIndexImg = TrapBallMaskAll(cmmsIndexImg);
    cv::Mat vecout = img.clone();
    vecout = cv::Scalar(0);
    cv::Mat ShowRegion = img.clone();
    //      Dilation(simg, 1, 2);
    for (int i = 0; i < simg.rows; ++i)
    {
        for (int j = 0; j < simg.cols; ++j)
        {
            if (simg.at<cv::Vec3b>(i, j)[2] > 0)
            {
                ShowRegion.at<cv::Vec3b>(i / 2, j / 2) = cv::Vec3b(255, 255, 255);
            }
        }
    }
    CvPatchs cps = S2_2GetPatchs(cmmsIndexImg, img, ccms2, vecout);
    cv::waitKey(10);
    // set draw patch line to Diffusion
    Lines patchlines = GetLinesFromCvPatch(cps);
    Color2Side patchColor = GetLinesColor(patchlines, ccms2);
    patchlines = SmoothingLen5(patchlines, 0.5, 5);
    // show patch img
    cv::Mat tmp = cmmsIndexImg.clone(), tmp2;
    cv::RNG rng(12345);
    tmp = cv::Scalar(0);
    tmp2 = tmp.clone();
    S6ReColor(cmmsIndexImg, img.clone(), ccms);
    Lines colorline = m_BlackLine2;//LineSplitAtIntersection(m_BlackLine2, 1);
    cv::Mat colorline2 = MakeColorLineImage(img, colorline);
    Index2Side i2s = GetLinesIndex2SideSmart(cmmsIndexImg, colorline, ccms);
    i2s.left = FixIndexs(i2s.left, 100);
    i2s.right = FixIndexs(i2s.right, 100);
    i2s.left = MedianLenN(i2s.left, 10);
    i2s.right = MedianLenN(i2s.right, 10);
    Color2Side color2s2 = LinesIndex2Color(colorline, i2s, ccms);
    d3dApp.AddDiffusionLines(colorline, color2s2);
    // draw red line
    m_DiffusionFrames.push_back(DiffusionFrame());
    m_DiffusionFrames.back().m_BlackLine = m_BlackLine;
    m_DiffusionFrames.back().m_BLineWidth = m_BLineWidth;
    m_DiffusionFrames.back().lineColors = lineColors;
    //m_DiffusionFrames.back().diffusionConstrant = diffusionConstrant;
    //m_DiffusionFrames.back().colors = colors;
    m_DiffusionFrames.back().m_BlackLine2 = colorline;
    m_DiffusionFrames.back().color2s = color2s2;
}

 */

const double UNKNOWN_FLOW_THRESH = 1e9;

std::vector<cv::Point> markerPoints;
std::vector<cv::Point> outPoints;
cv::Vec2f motion2Mask(cv::Mat flow)
{
	cv::Vec2f res;
	outPoints.clear();
	double sumx = 0, sumy = 0;
	int ct = 0;
	for(int i = 0; i < flow.rows; ++i)
	{
		for(int j = 0; j < flow.cols; ++j)
		{
			ct++;
			cv::Vec2f fp = flow.at<cv::Vec2f>(i, j);
			if((fabs(fp[0]) <  UNKNOWN_FLOW_THRESH))
			{
				sumx += fp[0];
			}
			if((fabs(fp[1]) <  UNKNOWN_FLOW_THRESH))
			{
				sumy += fp[1];
			}
		}
	}
	sumx /= ct;
	sumy /= ct;
	sumx *= 10;
	sumx = ((int)sumx) * 0.1;
	sumy *= 10;
	sumy = ((int)sumy) * 0.1;
	res[0] = sumx;
	res[1] = sumy;
	return res;
}

bool compareVec3b(const cv::Vec3b& a, const cv::Vec3b& b)
{
	if(a[0] < b[0])
	{
		return true;
	}
	if(a[0] > b[0])
	{
		return false;
	}
	if(a[1] < b[1])
	{
		return true;
	}
	if(a[1] > b[1])
	{
		return false;
	}
	if(a[2] < b[2])
	{
		return true;
	}
	if(a[2] > b[2])
	{
		return false;
	}
	return true;
}

void MakeStaticBackGround(Mats& m_Video, Vec2fs& m_Moves)
{
	if(m_Video.size() != m_Moves.size())
	{
		printf("m_Video.size() %d  m_Moves.size() %d\n", m_Video.size(), m_Moves.size());
		return;
	}
	else
	{
		int maxX = 0, maxY = 0, minX = 0, minY = 0;
		for(int i = 0; i < m_Moves.size(); ++i)
		{
			if(m_Moves[i][0] > maxX)
			{
				maxX = m_Moves[i][0] + 1;
			}
			if(m_Moves[i][1] > maxY)
			{
				maxY = m_Moves[i][1] + 1;
			}
			if(m_Moves[i][0] < minX)
			{
				minX = m_Moves[i][0] - 1;
			}
			if(m_Moves[i][1] < minY)
			{
				minY = m_Moves[i][1] - 1;
			}
		}
		int ow = m_Video.back().cols;
		int oh = m_Video.back().rows;
		cv::Mat bgimg, bgcount;
		Mats timgs;
		bgimg.create(oh + maxY - minY + 1, ow + maxX - minX + 1, CV_32FC3);
		bgcount.create(oh + maxY - minY + 1, ow + maxX - minX + 1, CV_32FC1);
		bgimg = cv::Scalar(0);
		bgcount = cv::Scalar(0);
		for(int a = 0; a < m_Video.size(); ++a)
		{
			cv::Mat timg;
			timg.create(oh + maxY - minY + 1, ow + maxX - minX + 1, CV_8UC3);
			timg = cv::Scalar(0);
			cv::Mat img = m_Video[a];
			cv::Vec2f mv = m_Moves[a];
			double x = mv[0] - minX;
			double y = mv[1] - minY;
			int fx = floor(x);
			int fy = floor(y);
			double lw = 1 - (x - floor(x));
			double rw = 1 - lw;
			double tw = 1 - (y - floor(y));
			double bw = 1 - tw;
			double lt = lw * tw, lb = lw * bw, rt = rw * tw, rb = rw * bw;
			printf("x %.2f y %.2f lw %.2f rw %.2f tw %.2f bw %.2f\n", x, y, lw, rw, tw, bw);
			for(int i = 0; i < img.rows; ++i)
			{
				for(int j = 0; j < img.cols; ++j)
				{
					cv::Vec3b& tv00 = timg.at<cv::Vec3b>(fy + i, fx + j);
					cv::Vec3b& tv10 = timg.at<cv::Vec3b>(fy + i + 1, fx + j);
					cv::Vec3b& tv01 = timg.at<cv::Vec3b>(fy + i, fx + j + 1);
					cv::Vec3b& tv11 = timg.at<cv::Vec3b>(fy + i + 1, fx + j + 1);
					cv::Vec3f& v00 = bgimg.at<cv::Vec3f>(fy + i, fx + j);
					cv::Vec3f& v10 = bgimg.at<cv::Vec3f>(fy + i + 1, fx + j);
					cv::Vec3f& v01 = bgimg.at<cv::Vec3f>(fy + i, fx + j + 1);
					cv::Vec3f& v11 = bgimg.at<cv::Vec3f>(fy + i + 1, fx + j + 1);
					float& c00 = bgcount.at<float>(fy + i, fx + j);
					float& c10 = bgcount.at<float>(fy + i + 1, fx + j);
					float& c01 = bgcount.at<float>(fy + i, fx + j + 1);
					float& c11 = bgcount.at<float>(fy + i + 1, fx + j + 1);
					c00 += lt;
					c10 += lb;
					c01 += rt;
					c11 += rb;
					v00[0] += img.at<cv::Vec3b>(i, j)[0] * lt;
					v00[1] += img.at<cv::Vec3b>(i, j)[1] * lt;
					v00[2] += img.at<cv::Vec3b>(i, j)[2] * lt;
					v10[0] += img.at<cv::Vec3b>(i, j)[0] * lb;
					v10[1] += img.at<cv::Vec3b>(i, j)[1] * lb;
					v10[2] += img.at<cv::Vec3b>(i, j)[2] * lb;
					v01[0] += img.at<cv::Vec3b>(i, j)[0] * rt;
					v01[1] += img.at<cv::Vec3b>(i, j)[1] * rt;
					v01[2] += img.at<cv::Vec3b>(i, j)[2] * rt;
					v11[0] += img.at<cv::Vec3b>(i, j)[0] * rb;
					v11[1] += img.at<cv::Vec3b>(i, j)[1] * rb;
					v11[2] += img.at<cv::Vec3b>(i, j)[2] * rb;
					tv00[0] += img.at<cv::Vec3b>(i, j)[0] * lt;
					tv00[1] += img.at<cv::Vec3b>(i, j)[1] * lt;
					tv00[2] += img.at<cv::Vec3b>(i, j)[2] * lt;
					tv10[0] += img.at<cv::Vec3b>(i, j)[0] * lb;
					tv10[1] += img.at<cv::Vec3b>(i, j)[1] * lb;
					tv10[2] += img.at<cv::Vec3b>(i, j)[2] * lb;
					tv01[0] += img.at<cv::Vec3b>(i, j)[0] * rt;
					tv01[1] += img.at<cv::Vec3b>(i, j)[1] * rt;
					tv01[2] += img.at<cv::Vec3b>(i, j)[2] * rt;
					tv11[0] += img.at<cv::Vec3b>(i, j)[0] * rb;
					tv11[1] += img.at<cv::Vec3b>(i, j)[1] * rb;
					tv11[2] += img.at<cv::Vec3b>(i, j)[2] * rb;
				}
			}
			timgs.push_back(timg);
		}
		cv::Mat meanimg;
		meanimg.create(oh + maxY - minY + 1, ow + maxX - minX + 1, CV_8UC3);
		for(int i = 0; i < bgimg.rows; ++i)
		{
			for(int j = 0; j < bgimg.cols; ++j)
			{
				Vec3bs colors;
				for(int k = 0; k < timgs.size(); ++k)
				{
					cv::Vec3b c = timgs[k].at<cv::Vec3b>(i, j);
					if(c != cv::Vec3b(0, 0, 0))
					{
						colors.push_back(c);
					}
				}
				if (!colors.empty())
				{
				std::sort(colors.begin(), colors.end(), compareVec3b);
				meanimg.at<cv::Vec3b>(i, j) = colors[colors.size()/2];
				}
			}
		}
		for(int i = 0; i < bgimg.rows; ++i)
		{
			for(int j = 0; j < bgimg.cols; ++j)
			{
				bgimg.at<cv::Vec3f>(i, j) /= bgcount.at<float>(i, j);
			}
		}
		cv::Mat showbg;
		bgimg.convertTo(showbg, CV_8UC3);
		//normalize(showbg, showbg, 0, 255, cv::NORM_MINMAX);
		cv::imshow("showbg", showbg);
		cv::imshow("meanimg", meanimg);
		cv::waitKey();
		printf("minX %d maxX %d minY %d maxY %d \n", minX, maxX, minY, maxY);
	}
}

void VAV_MainFrame::OnFileOpenVideo()
{
	CFileDialog dlg(TRUE);
	dlg.m_ofn.lpstrFilter   = L"All Files (*.*)\0*.*\0\0";
	dlg.m_ofn.lpstrTitle    = L"Load File";
	CString filename;
	if(dlg.DoModal() == IDOK)
	{
		filename = dlg.GetPathName(); // return full path and filename
		if(filename.GetLength() > 1)
		{
			m_Video.clear();
			m_Moves.clear();
			m_ReadVideo.Read(ConvStr::GetStr(filename.GetString()));
			cv::Mat img = m_ReadVideo.GetFrame();
			m_Video.push_back(img);
			m_Moves.push_back(cv::Vec2f(0, 0));
			cv::Mat prevgray, gray, flow, showflow;
			cvtColor(img, prevgray, CV_BGR2GRAY);
			for(int i = 0; i <= 30; ++i)
			{
				cv::Mat img = m_ReadVideo.GetFrame();
				if(img.cols > 0)
				{
					m_Video.push_back(img);
					cvtColor(img, gray, CV_BGR2GRAY);
					cv::calcOpticalFlowFarneback(prevgray, gray, flow,
												 0.25,   //pyrScale=0.5 means a classical pyramid
												 10,     //Number of pyramid layers including the initial image
												 400,     //Averaging window size. Larger values increase the algorithm robustness
												 10,     //Number of iterations the algorithm does at each pyramid level.
												 5,     //Size of the pixel neighborhood used to find polynomial expansion in each pixel,5/7
												 1.2,   //1.1/1.5
												 0);
					prevgray = gray.clone();
					//�ϥ�makerPoints,��XoutPoints
					cv::Vec2f move = motion2Mask(flow);//��markerMask trace
					m_Moves.push_back(-move + m_Moves.back());
					printf("avg_vel:%f,%f\n", m_Moves.back()[0], m_Moves.back()[1]);
					cv::Mat maker;
					maker.create(img.size(), CV_8UC1);
					maker = cv::Scalar(0);
					for(int idx = 0 ; idx < outPoints.size(); ++idx)
					{
						cv::Point sign = outPoints[idx];
						for(int i = -5 ; i < 6 ; ++i)
						{
							for(int j = -5 ; j < 6 ; ++j)
							{
								int x = sign.x + j;
								int y = sign.y + i;
								maker.at<uchar>(y, x) = 255;
							}
						}
					}
					cv::Mat res = maker * 0.2 + gray * 0.8;
//                  cv::imshow("StrokeMask", res);
//                  cv::waitKey(10);
					//AddFrame(img);
					printf("Frame %d\n", i);
				}
			}
			MakeStaticBackGround(m_Video, m_Moves);
		}
		//GetVavView()->SetTimer(100, 80, 0);
	}
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


void VAV_MainFrame::AddFrame(cv::Mat img)
{
	D3DApp& d3dApp = GetVavView()->GetD3DApp();
	d3dApp.SetPictureSize(img.cols, img.rows);
	vavImage vImage = img;
	vImage.ToExpImage();
	cv::Mat imgf, curveExtration, show3u = cv::Mat::zeros(vImage.GetCvMat().size(), CV_8UC3);
	imgf = vImage;
	cvtColor(imgf, imgf, CV_BGR2GRAY);
	imgf.convertTo(imgf, CV_32F, 1.0 / 255);
	CmCurveEx dEdge(imgf);
	dEdge.CalSecDer(7, 0.0001f);
	dEdge.Link();
	CvLines tpnts2d;
	const CEdges& edges = dEdge.GetEdges();
	for(size_t i = 0; i < edges.size(); i++)
	{
		cv::Vec3b color(rand() % 155 + 100, rand() % 155 + 100, rand() % 155 + 100);
		const std::vector<cv::Point>& pnts = edges[i].pnts;
		for(size_t j = 0; j < pnts.size(); j++)
		{
			show3u.at<cv::Vec3b>(pnts[j]) = color;
		}
		tpnts2d.push_back(edges[i].pnts);
	}
	m_BlackLine = GetLines(tpnts2d, 0, 0);
	m_BlackLine = SmoothingLen5(m_BlackLine);
	Lines showLines;
	Lines BLineWidth(m_BlackLine.size());
	Lines normals = GetNormalsLen2(m_BlackLine);
	const double blackRadio = 0.5;
	for(int idx1 = 0; idx1 < m_BlackLine.size(); ++idx1)
	{
		const Line& nowLine = m_BlackLine[idx1];
		const Line& nowNormals = normals[idx1];
		Line& lineWidths = BLineWidth[idx1];
		lineWidths.clear();
		for(int idx2 = 0; idx2 < nowLine.size() - 1; ++idx2)
		{
			const double LINE_WIDTH = 5;
			Vector2 start(nowLine[idx2] - nowNormals[idx2] * LINE_WIDTH);
			Vector2 end(nowLine[idx2] + nowNormals[idx2] * LINE_WIDTH);
			Vector2 start2(nowLine[idx2 + 1] - nowNormals[idx2 + 1] * LINE_WIDTH);
			Vector2 end2(nowLine[idx2 + 1] + nowNormals[idx2 + 1] * LINE_WIDTH);
			double_vector line1 = vImage.GetLineLight(start.x, start.y, end.x, end.y,
								  360);
			double_vector line2 = vImage.GetLineLight(start2.x, start2.y, end2.x, end2.y,
								  360);
			double_vector width1 = GetLineWidth(ConvertToSquareWave(ConvertToAngle(line1),
												15, 50), LINE_WIDTH * 2);
			double_vector width2 = GetLineWidth(ConvertToSquareWave(ConvertToAngle(line2),
												15, 50), LINE_WIDTH * 2);
			if(width1.size() >= 2 && width2.size() >= 2 && abs(width2[0] - width2[1]) < 1)
			{
				Line line1;
				line1.push_back(nowLine[idx2] - nowNormals[idx2] * width1[0] * blackRadio);
				line1.push_back(nowLine[idx2 + 1] - nowNormals[idx2 + 1] * width2[0] *
								blackRadio);
				line1 = GetLine(line1, 0.5, 0.5);
				Line line2;
				line2.push_back(nowLine[idx2] + nowNormals[idx2] * width1[1] * blackRadio);
				line2.push_back(nowLine[idx2 + 1] + nowNormals[idx2 + 1] * width2[1] *
								blackRadio);
				line2 = GetLine(line2, 0.5, 0.5);
				showLines.push_back(line1);
				showLines.push_back(line2);
// save line width
				lineWidths.push_back(Vector2(width1[0] * blackRadio, width1[1] * blackRadio));
			}
			else
			{
				lineWidths.push_back(Vector2());
			}
		}
		lineWidths.push_back(Vector2());
	}
	m_BLineWidth = BLineWidth;
	m_BlackLine = GetLines(tpnts2d, 0.0, 0.0);
	m_BlackLine = SmoothingLen5(m_BlackLine, 0.9, 5);
	LineEnds les = GetLineEnds(m_BlackLine);
	LinkLineEnds180(les, 5, 20);
	ConnectLineEnds(les, m_BlackLine, m_BLineWidth);
	IncreaseDensity(m_BlackLine, m_BLineWidth);
	les = GetLineEnds(m_BlackLine);
	ConnectNearestLines_Width(les, m_BlackLine, m_BLineWidth, 8, 15);
	m_BLineWidth = CleanOrphanedLineWidths(m_BLineWidth, 5);
	m_BLineWidth = FixLineWidths(m_BLineWidth, 50);
//m_BLineWidth = FixLineWidths(m_BLineWidth, 100);
	ClearLineWidthByPercent(m_BLineWidth, 0.4);
	m_BLineWidth = FixLineWidths(m_BLineWidth, 200);
	m_BLineWidth = SmoothingHas0Len5(m_BLineWidth, 0, 5);
	m_BlackLine = SmoothingLen5(m_BlackLine, 0, 5);
	Vector3s2d lineColors;
	lineColors = GetLinesColor(img, m_BlackLine);
	lineColors = SmoothingLen5(lineColors, 0, 10);
	m_BlackLine = GetLines(m_BlackLine, 1, 1);
	d3dApp.AddLinesWidth(m_BlackLine, m_BLineWidth, lineColors);
// block line
	dEdge.CalSecDer2(5, 0.001f);
	dEdge.Link();
	tpnts2d.clear();
	const CEdges& edges2 = dEdge.GetEdges();
	for(size_t i = 0; i < edges2.size(); i++)
	{
		cv::Vec3b color(rand() % 155 + 100, rand() % 155 + 100, rand() % 155 + 100);
		const std::vector<cv::Point>& pnts = edges2[i].pnts;
		for(size_t j = 0; j < pnts.size(); j++)
		{
			show3u.at<cv::Vec3b>(pnts[j]) = color;
		}
		tpnts2d.push_back(edges2[i].pnts);
	}
	m_BlackLine2 = GetLines(tpnts2d, 0, 0);
	Lines tmp_width = m_BlackLine2;
	m_BlackLine2 = SmoothingLen5(m_BlackLine2, 0.9, 5);
	les = GetLineEnds(m_BlackLine2);
	LinkLineEnds180(les, 5, 5);
	ConnectLineEnds(les, m_BlackLine2, tmp_width);
	IncreaseDensity(m_BlackLine2, tmp_width);
	les = GetLineEnds(m_BlackLine2);
	ConnectNearestLines_Width(les, m_BlackLine2, tmp_width, 5, 15);
//      les = GetLineEnds(m_BlackLine2);
//      ConnectNearestLines(les, m_BlackLine2, tmp_width, 10, 6, 20);
	m_BlackLine2 = SmoothingLen5(m_BlackLine2, 0.2, 5);
	d3dApp.AddLines(m_BlackLine2);
	d3dApp.SetScaleTemporary(1);
	d3dApp.BuildPoint();
	d3dApp.InterSetRenderTransparencyOutput2();
	cv::Mat simg = d3dApp.DrawSceneToCvMat();
	d3dApp.SetScaleRecovery();
	d3dApp.ClearSkeletonLines();
	cvtColor(simg, simg, CV_BGR2GRAY);
	curveExtration = simg.clone();
	Dilation(curveExtration, 2, 1);
	cvtColor(curveExtration, curveExtration, CV_GRAY2BGR);
	cv::Mat tmpimg = img.clone();
	cv::Mat sampleimg = img.clone();
	cv::GaussianBlur(tmpimg, tmpimg, cv::Size(5, 5), 0, 0);
	cv::GaussianBlur(tmpimg, tmpimg, cv::Size(5, 5), 0, 0);
	cv::Mat isoimg = MakeIsoSurfaceImg(tmpimg, 16);
	for(int i = 0; i < curveExtration.rows; i++)
	{
		for(int j = 0; j < curveExtration.cols; j++)
		{
			cv::Vec3b& v = curveExtration.at<cv::Vec3b>(i, j);
			if(v[0] > 0)
			{
				cv::Vec3b& sam = sampleimg.at<cv::Vec3b>(i, j);
				sam[0] = 0;
				sam[1] = 0;
				sam[2] = 0;
			}
		}
	}
	cvtColor(curveExtration, curveExtration, CV_BGR2GRAY);
	Dilation(curveExtration, 2, 2);
	cvtColor(curveExtration, curveExtration, CV_GRAY2BGR);
	for(int i = 0; i < curveExtration.rows; i++)
	{
		for(int j = 0; j < curveExtration.cols; j++)
		{
			cv::Vec3b& v = curveExtration.at<cv::Vec3b>(i, j);
			if(v[0] > 0)
			{
				cv::Vec3b& dst = isoimg.at<cv::Vec3b>(i, j);
				dst[0] = 1;
				dst[1] = 1;
				dst[2] = 1;
			}
		}
	}
	cv::imshow("sampleimg", sampleimg);
	Lines ColorWidth;
//Color2Side color2s = GetLinesColor2SideSmart3(img, sampleimg, m_BlackLine2, 30);
	Color2Side color2s = GetLinesColor2Side(img, m_BlackLine2, 2);
	color2s.left = FixLineColors(color2s.left, 600, 10);
	color2s.right = FixLineColors(color2s.right, 600, 10);
	les = GetLineEnds(m_BlackLine2);
	//ClearJointArea(les, m_BlackLine2, color2s, 8);
	color2s.left = FixLineColors(color2s.left, 600, 1);
	color2s.right = FixLineColors(color2s.right, 600, 1);
	color2s.left = MedianLen(color2s.left, 10, 3);
	color2s.right = MedianLen(color2s.right, 10, 3);
	color2s.left = SmoothingLen5(color2s.left, 0, 5);
	color2s.right = SmoothingLen5(color2s.right, 0, 5);
// edge extraction
	Lines diffusionConstrant = S6GetPatchs(isoimg, 0, 0);
	diffusionConstrant = SmoothingLen5(diffusionConstrant, 0, 1);
	Vector3s2d colors;
	colors = GetLinesColor(img, diffusionConstrant);
	diffusionConstrant = GetLines(diffusionConstrant, 0.5, 0.5);
//colors = FixLineColors(colors, 400, 10);
//colors = MedianLen(colors, 20, 3);
	colors = SmoothingLen5(colors, 0, 2);
// draw red line
	m_DiffusionFrames.push_back(DiffusionFrame());
	m_DiffusionFrames.back().m_BlackLine = m_BlackLine;
	m_DiffusionFrames.back().m_BLineWidth = m_BLineWidth;
	m_DiffusionFrames.back().lineColors = lineColors;
	m_DiffusionFrames.back().diffusionConstrant = diffusionConstrant;
	m_DiffusionFrames.back().colors = colors;
	m_DiffusionFrames.back().m_BlackLine2 = m_BlackLine2;
	m_DiffusionFrames.back().color2s = color2s;
}
