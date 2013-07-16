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

// VAV_View.cpp : VAV_View 類別的實作
//

#include "stdafx.h"
// SHARED_HANDLERS 可以定義在實作預覽、縮圖和搜尋篩選條件處理常式的
// ATL 專案中，並允許與該專案共用文件程式碼。
#ifndef SHARED_HANDLERS
#include "VAV_App.h"
#endif

#include "VAV_Doc.h"
#include "VAV_View.h"
#include "VAV_MainFrm.h"
#include "IFExtenstion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// VAV_View

IMPLEMENT_DYNCREATE(VAV_View, CView)

BEGIN_MESSAGE_MAP(VAV_View, CView)
	// 標準列印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &VAV_View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_MOUSEHWHEEL()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_CAPTURECHANGED()
	ON_WM_MOUSELEAVE()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCMBUTTONUP()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// VAV_View 建構/解構

VAV_View::VAV_View()
{
	// TODO: 在此加入建構程式碼
	m_MButtonDown = false;
	m_Scale = 1.f;
	// show historgram
	m_TimerCallback = vtkSmartNew;
	m_plot = vtkSmartNew;
	m_TimerCallback->m_plot = m_plot;
	m_renderWindow = vtkSmartNew;
	m_TimerCallback->m_renderWindow = m_renderWindow;
	m_interactor = vtkOnlyNew;
	m_TimerCallback->m_interactor = m_interactor;
	m_thread = (HANDLE)_beginthreadex(NULL, 0, MyThreadFunc, this, 0, NULL);
}

VAV_View::~VAV_View()
{
}

BOOL VAV_View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此經由修改 CREATESTRUCT cs
	// 達到修改視窗類別或樣式的目的
	return CView::PreCreateWindow(cs);
}

// VAV_View 描繪

void VAV_View::OnDraw(CDC* /*pDC*/)
{
	VAV_Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
	{
		return;
	}
	// TODO: 在此加入原生資料的描繪程式碼
	m_D3DApp.BuildPoint();
	m_D3DApp.DrawScene();
}


// VAV_View 列印


void VAV_View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL VAV_View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 預設的準備列印程式碼
	return DoPreparePrinting(pInfo);
}

void VAV_View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 加入列印前額外的初始設定
}

void VAV_View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 加入列印後的清除程式碼
}

void VAV_View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void VAV_View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y,
			this, TRUE);
#endif
}

void VAV_View::InitDx11(HWND hWnd)
{
	CRect rect;
	GetWindowRect(&rect);
	m_LookCenter.x = 0;
	m_LookCenter.y = 0;
	//  m_hWndDX11 = CreateWindowA("edit", "", WS_CHILD | WS_DISABLED | WS_VISIBLE
	//      , 0, 0, rect.right-rect.left, rect.bottom-rect.top, hWnd,
	//      (HMENU)"", 0, NULL);
	m_hWndDX11 = hWnd;
	::ShowWindow(m_hWndDX11, true);
	::UpdateWindow(m_hWndDX11);
	m_D3DApp.initApp(m_hWndDX11, rect.Width(), rect.Height());
	m_D3DApp.BuildShaderFX();
	m_D3DApp.SetLookCenter(m_LookCenter.x , m_LookCenter.y);
	vavImage::SetDx11Device(m_D3DApp.GetDevice(), m_D3DApp.GetDeviceContext());
}

VAV_MainFrame* VAV_View::GetMainFrame()
{
	return ((VAV_MainFrame*)GetParentFrame());
}

// VAV_View 診斷

#ifdef _DEBUG
void VAV_View::AssertValid() const
{
	CView::AssertValid();
}

void VAV_View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

VAV_Doc* VAV_View::GetDocument() const // 內嵌非偵錯版本
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(VAV_Doc)));
	return (VAV_Doc*)m_pDocument;
}
#endif //_DEBUG


// VAV_View 訊息處理常式

void VAV_View::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	InitDx11(GetSafeHwnd());
}

void VAV_View::SetTexture(ID3D11ShaderResourceView* tex)
{
	m_D3DApp.SetScale(m_Scale);
	m_D3DApp.SetTexture(tex);
	m_D3DApp.BuildPoint();
	m_D3DApp.DrawScene();
	m_ExpImage = GetImage()->Clone();
	m_ExpImage.ToExpImage();
}


void VAV_View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	m_D3DApp.OnResize(cx, cy);
}

void VAV_View::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CView::OnMouseHWheel(nFlags, zDelta, pt);
}

BOOL VAV_View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta > 0)
	{
		m_Scale += 0.5;
		printf("m_Scale %f\n", m_Scale);
	}
	else if (zDelta < 0)
	{
		m_Scale -= 0.25;
		printf("m_Scale %f\n", m_Scale);
	}
	m_D3DApp.SetScale(m_Scale);
	m_D3DApp.DrawScene();
	//cv::imwrite("draw.png", m_D3DApp.DrawSceneToCvMat());
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void VAV_View::OnMouseMove(UINT nFlags, CPoint point)
{
	CView::OnMouseMove(nFlags, point);
	if (m_MButtonDown)
	{
		//printf("%f %f\n", m_LookCenter.x, m_LookCenter.y);
		m_LookCenter.x = m_LookDown.x + point.x - m_MouseDown.x;
		m_LookCenter.y = m_LookDown.y + point.y - m_MouseDown.y;
		m_D3DApp.SetLookCenter(m_LookCenter.x , m_LookCenter.y);
	}
	if (m_LButtonDown)
	{
		m_MouseMove = point;
		ShowLineNormal();
	}
}

void VAV_View::OnMButtonDown(UINT nFlags, CPoint point)
{
	CView::OnMButtonDown(nFlags, point);
	m_D3DApp.SetLookCenter(m_LookCenter.x , m_LookCenter.y);
	m_MButtonDown = true;
	m_MouseDown = point;
	m_LookDown = m_LookCenter;
}

void VAV_View::OnMButtonUp(UINT nFlags, CPoint point)
{
	CView::OnMButtonUp(nFlags, point);
	m_D3DApp.SetLookCenter(m_LookCenter.x, m_LookCenter.y);
	m_MButtonDown = false;
	m_MouseUp = point;
}

void VAV_View::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_LButtonDown = true;
	m_MouseMove = point;
	CView::OnLButtonDown(nFlags, point);
	ShowLineNormal();
}
vavImage* VAV_View::GetImage()
{
	return &(GetMainFrame()->m_vavImage);
}
void VAV_View::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);
	// TODO: 在此加入您的訊息處理常式程式碼
	printf("OnMouseLeave\n");
}
void VAV_View::OnCaptureChanged(CWnd* pWnd)
{
	// TODO: 在此加入您的訊息處理常式程式碼
	CView::OnCaptureChanged(pWnd);
	printf("OnMouseLeave\n");
}
void VAV_View::OnMouseLeave()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	CView::OnMouseLeave();
	printf("OnMouseLeave\n");
}
void VAV_View::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	CView::OnNcMouseMove(nHitTest, point);
	printf("OnNcMouseMove\n");
	m_D3DApp.SetLookCenter(m_LookCenter.x, m_LookCenter.y);
	m_MButtonDown = false;
	m_MouseUp.x = m_LookDown.x;
	m_MouseUp.y = m_LookDown.y;
}
void VAV_View::OnNcMButtonUp(UINT nHitTest, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	CView::OnNcMButtonUp(nHitTest, point);
	printf("OnNcMButtonUp\n");
	m_D3DApp.SetLookCenter(m_LookCenter.x, m_LookCenter.y);
	m_MButtonDown = false;
	m_MouseUp.x = m_LookDown.x;
	m_MouseUp.y = m_LookDown.y;
}
D3DApp& VAV_View::GetD3DApp()
{
	return m_D3DApp;
}

void VAV_View::SetPictureSize(int w, int h)
{
	m_PicW = w;
	m_PicH = h;
	m_D3DApp.SetPictureSize(w, h);
}


void VAV_View::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	CView::OnLButtonUp(nFlags, point);
	m_LButtonDown = false;
}

void VAV_View::ShowLineNormal()
{
	double realX = (m_MouseMove.x - m_LookCenter.x) / m_Scale - m_LookCenter.x *
				   0.5;
	double realY = (m_PicH * m_Scale - m_D3DApp.Height() + m_MouseMove.y
					- m_LookCenter.y) / m_Scale - m_LookCenter.y * 0.5;
	printf("%3.2f %3.2f\n", realX, realY);
	m_D3DApp.SetLookCenter(m_LookCenter.x, m_LookCenter.y);
	Vector2 click(realX, realY);
	int idx1 = -1;
	int idx2 = -1;
	double dis = 5;
	int i = 0, j;
	for (auto it1 = m_FeatureLines.begin(); it1 != m_FeatureLines.end(); ++it1, ++i)
	{
		j = 0;
		for (auto it2 = it1->begin(); it2 != it1->end(); ++it2, ++j)
		{
			double nowDis = click.squaredDistance(*it2);
			if (nowDis < dis)
			{
				idx1 = i;
				idx2 = j;
				dis = nowDis;
			}
		}
	}
	if (idx1 != -1)
	{
		Line twoPoint;
		const double LINE_WIDTH = 3;
		Vector2 start(m_FeatureLines[idx1][idx2] - m_FeatureNormals[idx1][idx2] *
					  LINE_WIDTH);
		Vector2 end(m_FeatureLines[idx1][idx2] + m_FeatureNormals[idx1][idx2] *
					LINE_WIDTH);
		Vector2 start2(m_FeatureLines[idx1][idx2 + 1] -
					   m_FeatureNormals[idx1][idx2 + 1] * LINE_WIDTH);
		Vector2 end2(m_FeatureLines[idx1][idx2 + 1] +
					 m_FeatureNormals[idx1][idx2 + 1] * LINE_WIDTH);
		twoPoint.push_back(start);
		twoPoint.push_back(end);
		double_vector line1 = m_ExpImage.GetLineLight(start.x, start.y, end.x, end.y,
							  360);
		double_vector line2 = m_ExpImage.GetLineLight(start2.x, start2.y, end2.x,
							  end2.y,
							  360);
		line1 = SmoothingLen5(line1, 3);
		line2 = SmoothingLen5(line2, 3);
		m_TimerCallback->Lock();
		m_TimerCallback->m_data[0] = line1;
		m_TimerCallback->m_data[4] = ConvertToAngle(line1);
		m_TimerCallback->m_data[5] = ConvertToSquareWave(ConvertToAngle(line1), 5, 50);
		m_TimerCallback->Unlock();
		Lines push;
		push.push_back(twoPoint);
		double_vector width1 = GetLineWidth(ConvertToSquareWave(ConvertToAngle(line1),
											5, 50), LINE_WIDTH * 2);
		double_vector width2 = GetLineWidth(ConvertToSquareWave(ConvertToAngle(line2),
											5, 50), LINE_WIDTH * 2);
		if (width1.size() >= 2 && width2.size() >= 2)
		{
			Line line1;
			line1.push_back(m_FeatureLines[idx1][idx2] + m_FeatureNormals[idx1][idx2] *
							width1[0]);
			line1.push_back(m_FeatureLines[idx1][idx2 + 1] +
							m_FeatureNormals[idx1][idx2 + 1] * width2[0]);
			line1 = GetLine(line1, 0.5, 0.5);
			Line line2;
			line2.push_back(m_FeatureLines[idx1][idx2] + m_FeatureNormals[idx1][idx2] *
							width1[1]);
			line2.push_back(m_FeatureLines[idx1][idx2 + 1] +
							m_FeatureNormals[idx1][idx2 + 1] * width2[1]);
			line2 = GetLine(line2, 0.5, 0.5);
			push.push_back(line1);
			push.push_back(line2);
		}
		m_D3DApp.ClearCovers();
		m_D3DApp.AddLinesCover(push);
		m_D3DApp.BuildPoint();
		m_D3DApp.DrawScene();
	}
}

unsigned __stdcall VAV_View::MyThreadFunc(LPVOID lpParam)
{
	VAV_View* me = (VAV_View*)lpParam;
	me->m_plot->ExchangeAxesOff();
	me->m_plot->SetLabelFormat("%g");
	me->m_plot->SetXValuesToIndex();
	for (unsigned int i = 0 ; i < me->m_TimerCallback->m_data.size() ; i++)
	{
		vtkSmartPointer<vtkDoubleArray> array_s =
			vtkSmartPointer<vtkDoubleArray>::New();
		vtkSmartPointer<vtkFieldData> field =
			vtkSmartPointer<vtkFieldData>::New();
		vtkSmartPointer<vtkDataObject> data =
			vtkSmartPointer<vtkDataObject>::New();
		for (int b = 0; b < me->m_TimerCallback->m_data[i].size(); b++)
		{
			array_s->InsertValue(b, me->m_TimerCallback->m_data[i][b]);
		}
		field->AddArray(array_s);
		data->SetFieldData(field);
		me->m_plot->AddDataObjectInput(data);
	}
	me->m_plot->SetPlotColor(0, 1, 1, 1);
	me->m_plot->SetPlotColor(1, 1, 0, 0);
	me->m_plot->SetPlotColor(2, 0, 1, 0);
	me->m_plot->SetPlotColor(3, 0, 0, 1);
	me->m_plot->SetPlotColor(4, 1, 1, 1);
	me->m_plot->SetPlotColor(5, 1, 0, 0);
	me->m_plot->SetPlotColor(6, 0, 1, 0);
	me->m_plot->SetPlotColor(7, 0, 0, 1);
	me->m_plot->SetWidth(1);
	me->m_plot->SetHeight(1);
	me->m_plot->SetPosition(0, 0);
	me->m_plot->SetPlotRange(0, 0, 360, 400);
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(me->m_plot);
	me->m_renderWindow->AddRenderer(renderer);
	me->m_renderWindow->SetSize(500, 500);
	me->m_interactor->SetRenderWindow(me->m_renderWindow);
	// Initialize the event loop and then start it
	me->m_interactor->Initialize();
	// Sign up to receive TimerEvent
	me->m_interactor->AddObserver(vtkCommand::TimerEvent, me->m_TimerCallback);
	int timerId = me->m_interactor->CreateRepeatingTimer(100);
	std::cout << "timerId: " << timerId << std::endl;
	me->m_interactor->Start();
	return 0;
}
