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

// ImageFeatureDetection.cpp : 定義應用程式的類別行為。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "ImageFeatureDetection.h"
#include "MainFrm.h"

#include "ImageFeatureDetectionDoc.h"
#include "ImageFeatureDetectionView.h"
#include <auto_link_effect11.hpp>
#include <auto_link_opencv.hpp>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImageFeatureDetectionApp

BEGIN_MESSAGE_MAP(CImageFeatureDetectionApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CImageFeatureDetectionApp::OnAppAbout)
	// 依據文件命令的標準檔案
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 標準列印設定命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CImageFeatureDetectionApp 建構

CImageFeatureDetectionApp::CImageFeatureDetectionApp()
{
	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;

	m_bHiColorIcons = TRUE;

	// 支援重新啟動管理員
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果應用程式是使用 Common Language Runtime 支援 (/clr) 建置的:
	//     1) 要使重新啟動管理員支援正常運作需要這個額外設定。
	//     2) 在專案中必須將參考加入至 System.Windows.Forms 才能進行建置。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 以唯一的 ID 字串取代下面的應用程式 ID 字串; 建議的
	// 字串格式為 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("ImageFeatureDetection.AppID.NoVersion"));

	// TODO: 在此加入建構程式碼，
	// 將所有重要的初始設定加入 InitInstance 中
}

// 僅有的一個 CImageFeatureDetectionApp 物件

CImageFeatureDetectionApp theApp;


// CImageFeatureDetectionApp 初始設定

BOOL CImageFeatureDetectionApp::InitInstance()
{
	// 假如應用程式資訊清單指定使用 ComCtl32.dll 6 (含) 以後版本，
	// 來啟動視覺化樣式，在 Windows XP 上，則需要 InitCommonControls()。
	// 否則任何視窗的建立都將失敗。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 設定要包含所有您想要用於應用程式中的
	// 通用控制項類別。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// 初始化 OLE 程式庫
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// 需要有 AfxInitRichEdit2() 才能使用 RichEdit 控制項	
	// AfxInitRichEdit2();

	// 標準初始設定
	// 如果您不使用這些功能並且想減少
	// 最後完成的可執行檔大小，您可以
	// 從下列程式碼移除不需要的初始化常式，
	// 變更儲存設定值的登錄機碼
	// TODO: 您應該適度修改此字串
	// (例如，公司名稱或組織名稱)
	SetRegistryKey(_T("本機 AppWizard 所產生的應用程式"));
	LoadStdProfileSettings(4);  // 載入標準 INI 檔選項 (包含 MRU)


	InitContextMenuManager();
	InitShellManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 登錄應用程式的文件範本。文件範本負責在文件、
	// 框架視窗與檢視間進行連接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CImageFeatureDetectionDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架視窗
		RUNTIME_CLASS(CImageFeatureDetectionView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 剖析標準 Shell 命令、DDE、檔案開啟舊檔的命令列
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 在命令列中指定的分派命令。如果已使用 
	// /RegServer、/Register、/Unregserver 或 /Unregister 啟動應用程式，將傳回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 僅初始化一個視窗，所以顯示並更新該視窗
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

int CImageFeatureDetectionApp::ExitInstance()
{
	// TODO: 處理其他您已經加入的資源
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CImageFeatureDetectionApp 訊息處理常式


// 對 App About 使用 CAboutDlg 對話方塊

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 對話方塊資料
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

// 程式碼實作
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 執行對話方塊的應用程式命令
void CImageFeatureDetectionApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CImageFeatureDetectionApp 自訂載入/儲存方法

void CImageFeatureDetectionApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CImageFeatureDetectionApp::LoadCustomState()
{
}

void CImageFeatureDetectionApp::SaveCustomState()
{
}

// CImageFeatureDetectionApp 訊息處理常式



