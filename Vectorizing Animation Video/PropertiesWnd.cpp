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

#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "VAV_MainFrm.h"
#include "VAV_App.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar �T���B�z�`��

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient, rectCombo;
	GetClientRect(rectClient);
	m_wndObjectCombo.GetWindowRect(&rectCombo);
	int cyCmb = rectCombo.Size().cy;
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
	m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top,
	                              rectClient.Width(), 200, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb,
	                          rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left,
	                           rectClient.top + cyCmb + cyTlb, rectClient.Width(),
	                           rectClient.Height() - (cyCmb + cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	// �إߤU�Ԧ����:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER |
	                          CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("�L�k�إ� [�ݩ�] �U�Ԧ����\n");
		return -1;      // �L�k�إ�
	}

	m_wndObjectCombo.AddString(_T("���ε{��"));
	m_wndObjectCombo.AddString(_T("�ݩʵ���"));
	m_wndObjectCombo.SetCurSel(0);

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("�L�k�إ� [�ݩ�] ���\n");
		return -1;      // �L�k�إ�
	}

	InitPropList();
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* �w��w */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC :
	                        IDR_PROPERTIES, 0, 0, TRUE /* ��w */);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS |
	                          CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~
	                          (CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM |
	                           CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);
	// �Ҧ��R�O���N�g�Ѧ�����ǰe�A�Ӥ��O�g�Ѥ��ج[:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);
	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: �b���[�J�z���R�O�B�z�`���{���X
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: �b���[�J�z���R�O��s UI �B�z�`���{���X
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: �b���[�J�z���R�O�B�z�`���{���X
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: �b���[�J�z���R�O��s UI �B�z�`���{���X
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();
	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("�~�["));
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D �~�["),
	                    (_variant_t) false, _T("���w�������r�����D����A�B����� 3D �ؽu")));
	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("�ؽu"),
	        _T("Dialog Frame"), _T("�U�C�䤤�@��: None�BThin�BResizable �� Dialog Frame"));
	pProp->AddOption(_T("None"));
	pProp->AddOption(_T("Thin"));
	pProp->AddOption(_T("Resizable"));
	pProp->AddOption(_T("Dialog Frame"));
	pProp->AllowEdit(FALSE);
	pGroup1->AddSubItem(pProp);
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("���D"),
	                    (_variant_t) _T("����"), _T("���w��r�N��ܦb���������D�C")));
	m_wndPropList.AddProperty(pGroup1);
	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("�����j�p"),
	        0, TRUE);
	pProp = new CMFCPropertyGridProperty(_T("����"), (_variant_t) 250l,
	                                     _T("���w����������"));
	pProp->EnableSpinControl(TRUE, 50, 300);
	pSize->AddSubItem(pProp);
	pProp = new CMFCPropertyGridProperty(_T("�e��"), (_variant_t) 150l,
	                                     _T("���w�������e��"));
	pProp->EnableSpinControl(TRUE, 50, 200);
	pSize->AddSubItem(pProp);
	m_wndPropList.AddProperty(pSize);
	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("�r��"));
	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);
	lstrcpy(lf.lfFaceName, _T("Arial, �s�ө���"));
	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("�r��"), lf,
	                    CF_EFFECTS | CF_SCREENFONTS, _T("���w�������w�]�r��")));
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("�ϥΨt�Φr��"),
	                    (_variant_t) true, _T("���w�����ϥ� MS Shell Dlg �r��")));
	m_wndPropList.AddProperty(pGroup2);
	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("��L"));
	pProp = new CMFCPropertyGridProperty(_T("(�W��)"), _T("���ε{��"));
	pProp->Enable(FALSE);
	pGroup3->AddSubItem(pProp);
	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(
	    _T("������m"), RGB(210, 192, 254), NULL, _T("���w�w�]��������m"));
	pColorProp->EnableOtherButton(_T("��L..."));
	pColorProp->EnableAutomaticButton(_T("�w�]"), ::GetSysColor(COLOR_3DFACE));
	pGroup3->AddSubItem(pColorProp);
	static const TCHAR szFilter[] = _T("�ϥ���(*.ico)|*.ico|�Ҧ��ɮ�(*.*)|*.*||");
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("�ϥ�"), TRUE, _T(""),
	                    _T("ico"), 0, szFilter, _T("���w�����ϥ�")));
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("��Ƨ�"), _T("c:\\")));
	m_wndPropList.AddProperty(pGroup3);
	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("���h"));
	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(
	    _T("�Ĥ@�l�h��"));
	pGroup4->AddSubItem(pGroup41);
	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(
	    _T("�ĤG�l�h��"));
	pGroup41->AddSubItem(pGroup411);
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("���� 1"),
	                      (_variant_t) _T("�� 1"), _T("�o�O�y�z")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("���� 2"),
	                      (_variant_t) _T("�� 2"), _T("�o�O�y�z")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("���� 3"),
	                      (_variant_t) _T("�� 3"), _T("�o�O�y�z")));
	pGroup4->Expand(FALSE);
	m_wndPropList.AddProperty(pGroup4);
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());
	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);
	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);
	afxGlobalData.GetNonClientMetrics(info);
	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;
	m_fntPropList.CreateFontIndirect(&lf);
	m_wndPropList.SetFont(&m_fntPropList);
	m_wndObjectCombo.SetFont(&m_fntPropList);
}
