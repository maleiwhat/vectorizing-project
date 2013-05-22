// 這個 MFC 範例原始程式碼會示範如何使用 MFC Microsoft Office Fluent 使用者介面
// (「Fluent UI」)，並且僅提供為參考資料，做為
// MFC 參考及 MFC C++ 程式庫軟體
// 隨附相關電子文件的補充。
// 關於 Fluent UI 之複製、使用或散發的授權條款則分別提供。
// 如需 Fluent UI 授權計劃的詳細資訊，請造訪
// http://msdn.microsoft.com/officeui。
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// VAV_MainFrm.cpp : VAV_MainFrame 類別的實作
//

#include "stdafx.h"
#include "VAV_App.h"
#include "VAV_MainFrm.h"
#include "VAV_View.h"
#include "ConvStr.h"
#include "TriangulationCgal_Sideline.h"
#include "TriangulationCgal_Patch.h"
#include "VoronoiCgal_Patch.h"
#include "CvExtenstion.h"
#include "CvExtenstion2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// VAV_MainFrame

IMPLEMENT_DYNCREATE(VAV_MainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(VAV_MainFrame, CFrameWndEx)
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
	ON_COMMAND(ID_BUTTON_CGALTriangulation,
	           &VAV_MainFrame::OnButtonCGALTriangulation)
	ON_COMMAND(ID_BUTTON_Skeleton, &VAV_MainFrame::OnButtonSkeleton)
	ON_COMMAND(ID_BUTTON_Sobel, &VAV_MainFrame::OnButtonSobel)
	ON_COMMAND(ID_BUTTON_Laplace, &VAV_MainFrame::OnButtonLaplace)
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
END_MESSAGE_MAP()

// VAV_MainFrame 建構/解構
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

VAV_MainFrame::VAV_MainFrame()
{
	// TODO: 在此加入成員初始化程式碼
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"),
	                                  ID_VIEW_APPLOOK_OFF_2007_BLACK);
	AllocConsole();
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;
}

VAV_MainFrame::~VAV_MainFrame()
{
}

int VAV_MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	BOOL bNameValid;
	// 根據持續值設定視覺化管理員和樣式
	OnApplicationLook(theApp.m_nAppLook);
	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("無法建立狀態列\n");
		return -1;      // 無法建立
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
	// 啟用 Visual Studio 2005 樣式停駐視窗行為
	CDockingManager::SetDockingMode(DT_SMART);
	// 啟用 Visual Studio 2005 樣式停駐視窗自動隱藏行為
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 建立標題列:
	if (!CreateCaptionBar())
	{
		TRACE0("無法建立標題列\n");
		return -1;      // 無法建立
	}

	// 載入功能表項目影像 (不放在任何標準工具列上):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES,
	        theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 建立停駐視窗
	if (!CreateDockingWindows())
	{
		TRACE0("無法建立停駐視窗\n");
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
	if (!CFrameWndEx::PreCreateWindow(cs))
	{
		return FALSE;
	}

	// TODO: 在此經由修改 CREATESTRUCT cs
	// 達到修改視窗類別或樣式的目的
	return TRUE;
}

BOOL VAV_MainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	// 建立類別檢視
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);

	if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE,
	                           ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
	                           CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("無法建立 [類別檢視] 視窗\n");
		return FALSE; // 無法建立
	}

	// 建立檔案檢視
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);

	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE,
	                          ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
	                          CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("無法建立 [檔案檢視] 視窗\n");
		return FALSE; // 無法建立
	}

	// 建立輸出視窗
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);

	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE,
	                        ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
	                        CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("無法建立 [輸出] 視窗\n");
		return FALSE; // 無法建立
	}

	// 建立屬性視窗
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);

	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE,
	                            ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
	                            | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("無法建立 [屬性] 視窗\n");
		return FALSE; // 無法建立
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
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this,
	                            ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("無法建立標題列\n");
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

// VAV_MainFrame 診斷

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


// VAV_MainFrame 訊息處理常式

void VAV_MainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;
	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
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
		switch (theApp.m_nAppLook)
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
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void VAV_MainFrame::OnFilePrintPreview()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND,
		            AFX_ID_PREVIEW_CLOSE);   // 強制預覽列印模式關閉
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
	// TODO: 在此加入您的命令處理常式程式碼
}

void VAV_MainFrame::OnFileOpenVideo()
{
	CFileDialog dlg(TRUE);
	dlg.m_ofn.lpstrFilter   = L"All Files (*.*)\0*.*\0\0";
	dlg.m_ofn.lpstrTitle    = L"Load File";
	CString filename;

	if (dlg.DoModal() == IDOK)
	{
		filename = dlg.GetPathName(); // return full path and filename

		if (filename.GetLength() > 1)
		{
		}
	}
}

void VAV_MainFrame::OnFileOpenPicture()
{
	CFileDialog dlg(TRUE);
	dlg.m_ofn.lpstrFilter   = L"All Files (*.*)\0*.*\0\0";
	dlg.m_ofn.lpstrTitle    = L"Load File";
	CString filename;

	if (dlg.DoModal() == IDOK)
	{
		filename = dlg.GetPathName(); // return full path and filename

		if (filename.GetLength() > 1)
		{
			D3DApp& d3dApp = ((VAV_View*)this->GetActiveView())->GetD3DApp();
			CMFCRibbonEdit* re;
			CMFCRibbonBaseElement* tmp_ui = m_wndRibbonBar.GetCategory(2)->FindByID(
			                                    ID_SPIN_TransparencySelectPatch);
			re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

			if (NULL != re)
			{
				m_SelectPatchTransparency = atoi(ConvStr::GetStr(
				                                     re->GetEditText().GetString()).c_str());
			}

			d3dApp.SetTransparency_SelectPatch((100 - m_SelectPatchTransparency) * 0.01);
			tmp_ui = m_wndRibbonBar.GetCategory(2)->FindByID(ID_SPIN_TransparencyPatch);
			re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

			if (NULL != re)
			{
				m_PatchTransparency = atoi(ConvStr::GetStr(
				                               re->GetEditText().GetString()).c_str());
			}

			d3dApp.SetTransparency_SelectPatch((100 - m_PatchTransparency) * 0.01);
			tmp_ui = m_wndRibbonBar.GetCategory(2)->FindByID(
			             ID_SPIN_TransparencyTriangleLine);
			re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

			if (NULL != re)
			{
				m_TriangleLineTransparency = atoi(ConvStr::GetStr(
				                                      re->GetEditText().GetString()).c_str());
			}

			d3dApp.SetTransparency_SelectPatch((100 - m_TriangleLineTransparency) * 0.01);
			tmp_ui = m_wndRibbonBar.GetCategory(2)->FindByID(ID_SPIN_TransparencyPicture);
			re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

			if (NULL != re)
			{
				m_PictureTransparency = atoi(ConvStr::GetStr(
				                                 re->GetEditText().GetString()).c_str());
			}

			d3dApp.SetTransparency_Picture((100 - m_PictureTransparency) * 0.01);
			m_vavImage.ReadImage(ConvStr::GetStr(filename.GetString()));
			d3dApp.ClearTriangles();
			d3dApp.SetPictureSize(m_vavImage.GetWidth(), m_vavImage.GetHeight());
			((VAV_View*)this->GetActiveView())->SetTexture(m_vavImage.GetDx11Texture());
		}
	}
}

void VAV_MainFrame::OnButtonCanny()
{
	if (!m_vavImage.Vaild()) { return; }

	// TODO: 在此加入您的命令處理常式程式碼
	int t1 = 0, t2 = 30, a = 3;
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = m_wndRibbonBar.GetCategory(1)->FindByID(
	                                    ID_SPIN_CannyThreshold1);
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

	if (NULL != re)
	{
		t1 = atoi(ConvStr::GetStr(re->GetEditText().GetString()).c_str());
	}

	tmp_ui = m_wndRibbonBar.GetCategory(1)->FindByID(ID_SPIN_CannyThreshold2);
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

	if (NULL != re)
	{
		t2 = atoi(ConvStr::GetStr(re->GetEditText().GetString()).c_str());
	}

	tmp_ui = m_wndRibbonBar.GetCategory(1)->FindByID(ID_SPIN_ApertureSize);
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

	if (NULL != re)
	{
		a = atoi(ConvStr::GetStr(re->GetEditText().GetString()).c_str());
	}

	//  m_cannyImage = m_vavImage;
	//cv::Mat ce = CannyEdge(m_vavImage, t1, t2, a);
	//Lines lines1 = ComputeEdgeLine(ce);
	//  Lines lines = m_vavImage.AnimaEdge(5, 0.01, 0.1);
	//  lines = ComputeTrappedBallEdge(m_vavImage, lines, 5);
	//  cv::Mat lineImage = MakeLineImage(m_vavImage, lines);
	//  lines = GetLines(GetSidelines(lineImage));
	//  m_cannyImage.ShowEdgeLine(lines);
	m_cannyImage = m_vavImage;
	//Skeleton(m_cannyImage);
	cv::Mat cw, cw2;
	cw2 = m_cannyImage;
	cw2.convertTo(cw, CV_32FC3);
	Collect_Water(cw, cw, 5, 5, m_BlackRegionThreshold * 0.01);
	m_cannyImage = cw;
	((VAV_View*)this->GetActiveView())->SetTexture(m_cannyImage.GetDx11Texture());
}


void VAV_MainFrame::OnSpinCannyThreshold1()
{
	// TODO: 在此加入您的命令處理常式程式碼
}


void VAV_MainFrame::OnSpinCannyThreshold2()
{
	// TODO: 在此加入您的命令處理常式程式碼
}


void VAV_MainFrame::OnSpinAperturesize()
{
	// TODO: 在此加入您的命令處理常式程式碼
}


void VAV_MainFrame::OnButtonControlPointInitialize()
{
	// TODO: 在此加入您的命令處理常式程式碼
}

void VAV_MainFrame::OnButtonCGALTriangulation()
{
	D3DApp& d3dApp = ((VAV_View*)this->GetActiveView())->GetD3DApp();
	const bool DRAW_PATCH = false;
	const bool DRAW_SEPARATE_PATCH = false;
	const bool DRAW_CONTOUR = true;
	const bool DRAW_CONTOUR_CONTROL_POINT = false;
	const bool DRAW_CONTOUR_SKELETON_POINT = false;
	//Lines lines1 = m_vavImage.AnimaEdge(5, 0.01, 0.1);
	//Lines lines = ComputeTrappedBallEdge(m_vavImage, lines1, 5);
	//cv::Mat lineImage = MakeLineImage(m_vavImage, lines);
	//m_CvPatchs = S1GetPatchs(lineImage, 1, 10);
	//m_CvPatchs = S2GetPatchs(m_vavImage, 0, 0);
	ImageSpline is;

	if (DRAW_PATCH || DRAW_SEPARATE_PATCH)
	{
		is = S3GetPatchs(m_vavImage, 0, 0);
	}

	if (DRAW_PATCH)
	{
		TriangulationCgal_Patch cgal_patch;
		cgal_patch.SetSize(m_vavImage.GetWidth(), m_vavImage.GetHeight());
		cgal_patch.AddImageSpline(is);

		for (int i = 0; i < is.m_CvPatchs.size(); ++i)
		{
			is.m_CvPatchs[i].SetImage(m_vavImage);
			ColorConstraint_sptr constraint_sptr = is.m_CvPatchs[i].GetColorConstraint();
			cgal_patch.AddColorConstraint(constraint_sptr);
		}

		cgal_patch.SetCriteria(0.0, 4000);
		cgal_patch.Compute();
		d3dApp.AddColorTriangles(cgal_patch.GetTriangles());
		d3dApp.AddTrianglesLine(cgal_patch.GetTriangles());
	}

	// separate patch
	if (DRAW_SEPARATE_PATCH)
	{
		for (int i = 0; i < is.m_CvPatchs.size(); ++i)
		{
			TriangulationCgal_Patch cgal_patch;
			cgal_patch.SetSize(m_vavImage.GetWidth(), m_vavImage.GetHeight());
			Patch t_patch = ToPatch(is.m_CvPatchs[i]);
			t_patch.SmoothPatch();
			cgal_patch.AddPatch(t_patch);
			is.m_CvPatchs[i].SetImage(m_vavImage);
			ColorConstraint_sptr constraint_sptr = is.m_CvPatchs[i].GetColorConstraint();
			cgal_patch.AddColorConstraint(constraint_sptr);
			cgal_patch.SetCriteria(0.0, 4000);
			cgal_patch.Compute();
			d3dApp.AddColorTriangles(cgal_patch.GetTriangles());
			d3dApp.AddTrianglesLine(cgal_patch.GetTriangles());
		}
	}

	TriangulationCgal_Sideline cgal_contour;
	ImageSpline is2;

	if (DRAW_CONTOUR)
	{
		is2 = ComputeLines(m_vavImage, m_BlackRegionThreshold * 0.01);
	}

	// contour
	if (DRAW_CONTOUR)
	{
		cgal_contour.SetSize(m_vavImage.GetWidth(), m_vavImage.GetHeight());
		cgal_contour.AddImageSpline(is2);
		cgal_contour.SetCriteria(0.001, 4000);
		int region = cgal_contour.Compute();
		printf("region: %d\n", region);
		Vector3s2d colors = GetLinesColor(m_vavImage, cgal_contour.m_OriginLines);

		// add begin end line
		for (int i = 0; i < colors.size(); ++i)
		{
			Vector3s& cps = colors[i];

			if (cps.size() > 2)
			{
				Vector3s addcps;
				Vector3 front = cps.front();
				Vector3 back = cps.back();
				addcps.push_back(front);
				addcps.push_back(front);
				addcps.push_back(front);
				cps.insert(cps.begin(), addcps.begin(), addcps.end());
				cps.push_back(back);
				cps.push_back(back);
				cps.push_back(back);
			}
		}

		for (int count = 0; count < 2; count++)
		{
			for (int i = 0; i < colors.size(); ++i)
			{
				Vector3s& cps = colors[i];
				Vector3s newcps;

				if (cps.size() < 4) { continue; }

				newcps.push_back(cps.front());
				newcps.push_back(*(cps.begin() + 1));

				for (int j = 2; j < cps.size() - 2; j ++)
				{
					auto vec = (cps[j] * 2 + cps[j + 1] + cps[j - 1] + cps[j + 2] + cps[j - 2]) /
					           6.0f;
					newcps.push_back(vec);
				}

				newcps.push_back(*(cps.end() - 2));
				newcps.push_back(cps.back());
				cps = newcps;
			}
		}

		cgal_contour.MakeColorSequential();
		d3dApp.AddColorTriangles(cgal_contour.GetTriangles());
		d3dApp.AddTrianglesLine(cgal_contour.GetTriangles());
		d3dApp.SetScaleTemporary(1);
		d3dApp.BuildPoint();
		cv::Mat simg = d3dApp.DrawSceneToCvMat();
		d3dApp.SetScaleRecovery();
		ColorConstraint_sptrs RegionColors = MakeColors(region, simg, m_vavImage);
		d3dApp.ClearTriangles();
		cgal_contour.SetColor(RegionColors);
		d3dApp.AddColorTriangles(cgal_contour.GetTriangles());
		d3dApp.AddTrianglesLine(cgal_contour.GetTriangles());
//		d3dApp.AddLineSegs(cgal_contour.m_LineSegs);
		d3dApp.AddLines(cgal_contour.m_Lines, cgal_contour.m_LinesWidth, colors);
		d3dApp.AddLines(cgal_contour.m_ContourLines);

		if (DRAW_CONTOUR_CONTROL_POINT)
		{
			// Control Points
			for (int i = 0; i < cgal_contour.m_Controls.size(); ++i)
			{
				D3DXVECTOR3 color;
				color.x = 0.5 + 0.5 * rand() / (float)RAND_MAX;
				color.y = 0.5 + 0.5 * rand() / (float)RAND_MAX;
				color.z = 0.5 + 0.5 * rand() / (float)RAND_MAX;
				Line& cps = cgal_contour.m_Controls[i];

				for (int j = 0; j < cps.size(); ++j)
				{
					d3dApp.AddBigPoint(cps[j].x, cps[j].y, color);
				}
			}
		}

		if (DRAW_CONTOUR_SKELETON_POINT)
		{
			for (int i = 0; i < is2.m_Controls.size(); ++i)
			{
				D3DXVECTOR3 color;
				color.x = 0.5 + 0.5 * rand() / (float)RAND_MAX;
				color.y = 0.5 + 0.5 * rand() / (float)RAND_MAX;
				color.z = 0.5 + 0.5 * rand() / (float)RAND_MAX;
				Line& cps = is2.m_Controls[i];

				for (int j = 0; j < cps.size(); ++j)
				{
					d3dApp.AddBigPoint(cps[j].x, cps[j].y, color);
				}
			}
		}
	}

	d3dApp.BuildPoint();
	d3dApp.DrawScene();
}


void VAV_MainFrame::OnButtonSkeleton()
{
	// TODO: 在此加入您的命令處理常式程式碼
	m_cannyImage = m_vavImage;
	//Skeleton(m_cannyImage);
	cv::Mat cw = cv::Mat(m_cannyImage).clone();

	for (int i = 0; i < cw.rows; i++)
	{
		for (int j = 0; j < cw.cols; j++)
		{
			cv::Vec3b& v = cw.at<cv::Vec3b>(i, j);
			v[0] = 255;
			v[1] = 255;
			v[2] = 255;
		}
	}

	m_cannyImage = cw;
	((VAV_View*)this->GetActiveView())->SetTexture(m_cannyImage.GetDx11Texture());
}


void VAV_MainFrame::OnButtonSobel()
{
	// TODO: 在此加入您的命令處理常式程式碼
	m_cannyImage = m_vavImage;
	((VAV_View*)this->GetActiveView())->SetTexture(m_cannyImage.GetDx11Texture());
}


void VAV_MainFrame::OnButtonLaplace()
{
	// TODO: 在此加入您的命令處理常式程式碼
}


void VAV_MainFrame::OnSpinTransparencySelectPatch()
{
	((VAV_View*)this->GetActiveView())->
	GetD3DApp().SetTransparency_SelectPatch((100 - m_SelectPatchTransparency) *
	                                        0.01);
}


void VAV_MainFrame::OnUpdateSpinTransparencySelectPatch(CCmdUI* pCmdUI)
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = m_wndRibbonBar.GetCategory(2)->FindByID(
	                                    ID_SPIN_TransparencySelectPatch);
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

	if (NULL != re)
	{
		m_SelectPatchTransparency = atoi(ConvStr::GetStr(
		                                     re->GetEditText().GetString()).c_str());
	}

	((VAV_View*)this->GetActiveView())->
	GetD3DApp().SetTransparency_SelectPatch((100 - m_SelectPatchTransparency) *
	                                        0.01);
}

void VAV_MainFrame::ShowPatch(double x, double y)
{
	((VAV_View*)this->GetActiveView())->GetD3DApp().ClearPatchs();

	for (int i = 0; i < m_CvPatchs.size(); ++i)
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
	CMFCRibbonBaseElement* tmp_ui = m_wndRibbonBar.GetCategory(2)->FindByID(
	                                    ID_SPIN_TransparencyPatch);
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

	if (NULL != re)
	{
		m_PatchTransparency = atoi(ConvStr::GetStr(
		                               re->GetEditText().GetString()).c_str());
	}

	((VAV_View*)this->GetActiveView())->GetD3DApp().SetTransparency_Triangle((
	            100 - m_PatchTransparency) * 0.01);
}


void VAV_MainFrame::OnUpdateSpinTransparencyPatch(CCmdUI* pCmdUI)
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = m_wndRibbonBar.GetCategory(2)->FindByID(
	                                    ID_SPIN_TransparencyPatch);
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

	if (NULL != re)
	{
		m_PatchTransparency = atoi(ConvStr::GetStr(
		                               re->GetEditText().GetString()).c_str());
	}

	((VAV_View*)this->GetActiveView())->GetD3DApp().SetTransparency_Triangle((
	            100 - m_PatchTransparency) * 0.01);
}


void VAV_MainFrame::OnSpinTransparencyline()
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = m_wndRibbonBar.GetCategory(2)->FindByID(
	                                    ID_SPIN_TransparencyLine);
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

	if (NULL != re)
	{
		m_LineTransparency = atoi(ConvStr::GetStr(
		                              re->GetEditText().GetString()).c_str());
	}

	((VAV_View*)this->GetActiveView())->GetD3DApp().SetTransparency_Line((
	            100 - m_LineTransparency) * 0.01);
}

void VAV_MainFrame::OnUpdateSpinTransparencyline(CCmdUI* pCmdUI)
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = m_wndRibbonBar.GetCategory(2)->FindByID(
	                                    ID_SPIN_TransparencyLine);
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

	if (NULL != re)
	{
		m_LineTransparency = atoi(ConvStr::GetStr(
		                              re->GetEditText().GetString()).c_str());
	}

	((VAV_View*)this->GetActiveView())->GetD3DApp().SetTransparency_Line((
	            100 - m_LineTransparency) * 0.01);
}


void VAV_MainFrame::OnUpdateSpinTransparencypicture(CCmdUI* pCmdUI)
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = m_wndRibbonBar.GetCategory(2)->FindByID(
	                                    ID_SPIN_TransparencyPicture);
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

	if (NULL != re)
	{
		m_PictureTransparency = atoi(ConvStr::GetStr(
		                                 re->GetEditText().GetString()).c_str());
	}

	((VAV_View*)this->GetActiveView())->GetD3DApp().SetTransparency_Picture((
	            100 - m_PictureTransparency) * 0.01);
}


void VAV_MainFrame::OnSpinTransparencypicture()
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = m_wndRibbonBar.GetCategory(2)->FindByID(
	                                    ID_SPIN_TransparencyPicture);
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

	if (NULL != re)
	{
		m_PictureTransparency = atoi(ConvStr::GetStr(
		                                 re->GetEditText().GetString()).c_str());
	}

	((VAV_View*)this->GetActiveView())->GetD3DApp().SetTransparency_Picture((
	            100 - m_PictureTransparency) * 0.01);
}


void VAV_MainFrame::OnSpinBlackregionthreshold()
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = m_wndRibbonBar.GetCategory(2)->FindByID(
	                                    ID_SPIN_BlackRegionThreshold);
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

	if (NULL != re)
	{
		m_BlackRegionThreshold = atoi(ConvStr::GetStr(
		                                  re->GetEditText().GetString()).c_str());
	}
}


void VAV_MainFrame::OnUpdateSpinBlackregionthreshold(CCmdUI* pCmdUI)
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = m_wndRibbonBar.GetCategory(2)->FindByID(
	                                    ID_SPIN_BlackRegionThreshold);
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

	if (NULL != re)
	{
		m_BlackRegionThreshold = atoi(ConvStr::GetStr(
		                                  re->GetEditText().GetString()).c_str());
	}
}


void VAV_MainFrame::OnSpinTransparencytriangleline()
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = m_wndRibbonBar.GetCategory(2)->FindByID(
	                                    ID_SPIN_TransparencyTriangleLine);
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

	if (NULL != re)
	{
		m_TriangleLineTransparency = atoi(ConvStr::GetStr(
		                                      re->GetEditText().GetString()).c_str());
	}

	((VAV_View*)this->GetActiveView())->GetD3DApp().SetTransparency_TriangleLine((
	            100 - m_TriangleLineTransparency) * 0.01);
}


void VAV_MainFrame::OnUpdateSpinTransparencytriangleline(CCmdUI* pCmdUI)
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = m_wndRibbonBar.GetCategory(2)->FindByID(
	                                    ID_SPIN_TransparencyTriangleLine);
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

	if (NULL != re)
	{
		m_TriangleLineTransparency = atoi(ConvStr::GetStr(
		                                      re->GetEditText().GetString()).c_str());
	}

	((VAV_View*)this->GetActiveView())->GetD3DApp().SetTransparency_TriangleLine((
	            100 - m_TriangleLineTransparency) * 0.01);
}


void VAV_MainFrame::OnSpinTransparencylineskeleton()
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = m_wndRibbonBar.GetCategory(2)->FindByID(
	                                    ID_SPIN_TransparencyLineSkeleton);
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

	if (NULL != re)
	{
		m_LineSkeletonTransparency = atoi(ConvStr::GetStr(
		                                      re->GetEditText().GetString()).c_str());
	}

	((VAV_View*)this->GetActiveView())->GetD3DApp().SetTransparency_LineSkeleton((
	            100 - m_LineSkeletonTransparency) * 0.01);
}

void VAV_MainFrame::OnUpdateSpinTransparencylineskeleton(CCmdUI* pCmdUI)
{
	CMFCRibbonEdit* re;
	CMFCRibbonBaseElement* tmp_ui = m_wndRibbonBar.GetCategory(2)->FindByID(
	                                    ID_SPIN_TransparencyLineSkeleton);
	re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);

	if (NULL != re)
	{
		m_LineSkeletonTransparency = atoi(ConvStr::GetStr(
		                                      re->GetEditText().GetString()).c_str());
	}

	((VAV_View*)this->GetActiveView())->GetD3DApp().SetTransparency_LineSkeleton((
	            100 - m_LineSkeletonTransparency) * 0.01);
}
