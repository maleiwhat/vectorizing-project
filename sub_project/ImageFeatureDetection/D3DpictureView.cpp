// D3DpictureView.cpp : 實作檔
//

#include "stdafx.h"
#include "D3DpictureView.h"
#include "MainFrm.h"
// CD3DpictureView
ViewMap g_ViewMap;

IMPLEMENT_DYNCREATE(CD3DpictureView, CView)
CD3DpictureView* g_NewPictureView = NULL;

CD3DpictureView::CD3DpictureView():
	m_MButtonDown(false), m_Scale(1), m_LButtonDown(false)
{
	m_LineRadius = 5;
	g_NewPictureView = this;
	m_vavImage = NULL;
	m_hsvImage = NULL;
	m_TimerCallback = vtkSmartNew;
	m_plot = vtkSmartNew;
	m_TimerCallback->m_plot = m_plot;
	m_renderWindow = vtkSmartNew;
	m_TimerCallback->m_renderWindow = m_renderWindow;
	m_interactor = vtkOnlyNew;
	m_TimerCallback->m_interactor = m_interactor;
	m_thread = (HANDLE)_beginthreadex(NULL, 0, MyThreadFunc, this, 0, NULL);
}

CD3DpictureView::~CD3DpictureView()
{
	m_TimerCallback->Stop();
	if (m_vavImage)
	{
		delete m_vavImage;
	}
	if (m_hsvImage)
	{
		delete m_hsvImage;
	}
	WaitForSingleObject(
		m_thread,    // handle to mutex
		INFINITE);  // no time-out interval
}

BEGIN_MESSAGE_MAP(CD3DpictureView, CView)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSELEAVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CD3DpictureView 診斷

#ifdef _DEBUG
void CD3DpictureView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CD3DpictureView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif
#endif //_DEBUG

// CD3DpictureView 描繪

void CD3DpictureView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此加入描繪程式碼
	m_D3DApp.BuildPoint();
	m_D3DApp.DrawScene();
}

void CD3DpictureView::InitDx11(HWND hWnd)
{
	CRect rect;
	GetWindowRect(&rect);
	m_hWndDX11 = hWnd;
	::ShowWindow(m_hWndDX11, true);
	::UpdateWindow(m_hWndDX11);
	m_D3DApp.initApp(m_hWndDX11, rect.Width(), rect.Height());
	m_D3DApp.BuildShaderFX();
	m_D3DApp.SetLineRadius(m_LineRadius);
}

// CD3DpictureView 訊息處理常式

void CD3DpictureView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	//InitDx11(this->GetParent()->GetParent()->GetParent()->GetSafeHwnd());
	this->InitDx11(this->GetSafeHwnd());
}

int CD3DpictureView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	// TODO:  在此加入特別建立的程式碼
	return 0;
}

void CD3DpictureView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	// TODO: 在此加入您的訊息處理常式程式碼
	if (cx > 0 && cy > 0)
	{
		m_D3DApp.OnResize(cx, cy);
	}
}

void CD3DpictureView::OnMouseMove(UINT nFlags, CPoint point)
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
		UpdateImageFeature();
	}
}


BOOL CD3DpictureView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if (zDelta > 0)
	{
		m_Scale += 0.1;
	}
	else
	{
		m_Scale -= 0.1;
	}
	m_D3DApp.SetScale(m_Scale);
	m_D3DApp.BuildPoint();
	m_D3DApp.DrawScene();
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CD3DpictureView::OnMouseLeave()
{
	m_D3DApp.SetLookCenter(m_LookCenter.x, m_LookCenter.y);
	m_MButtonDown = false;
	m_MouseUp.x = m_LookDown.x;
	m_MouseUp.y = m_LookDown.y;
	m_LButtonDown = false;
	CView::OnMouseLeave();
}


void CD3DpictureView::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_D3DApp.SetLookCenter(m_LookCenter.x , m_LookCenter.y);
	m_MButtonDown = true;
	m_MouseDown = point;
	m_LookDown = m_LookCenter;
	CView::OnMButtonDown(nFlags, point);
}


void CD3DpictureView::OnMButtonUp(UINT nFlags, CPoint point)
{
	m_D3DApp.SetLookCenter(m_LookCenter.x, m_LookCenter.y);
	m_MButtonDown = false;
	m_MouseUp = point;
	CView::OnMButtonUp(nFlags, point);
}

void CD3DpictureView::OnPaint()
{
	CPaintDC dc(this);   // device context for painting
	CRect rect;
	GetClientRect(&rect);
	m_D3DApp.OnResize(rect.Width(), rect.Height());
	m_D3DApp.BuildPoint();
	m_D3DApp.DrawScene();
}

void CD3DpictureView::Init()
{
}

void CD3DpictureView::Update(int x, int y)
{
}

void CD3DpictureView::PreBuild(int x, int y)
{
}

void CD3DpictureView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_LButtonDown = true;
	CView::OnLButtonDown(nFlags, point);
	UpdateImageFeature();
}


void CD3DpictureView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_LButtonDown = false;
	CView::OnLButtonUp(nFlags, point);
	m_MouseMove = point;
}

unsigned __stdcall CD3DpictureView::MyThreadFunc(LPVOID lpParam)
{
	CD3DpictureView* me = (CD3DpictureView*)lpParam;
	me->m_plot->ExchangeAxesOff();
	me->m_plot->SetLabelFormat("%g");
	//  me->m_plot->SetXTitle( "Level" );
	//  me->m_plot->SetYTitle( "Frequency" );
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
	me->m_plot->SetWidth(1);
	me->m_plot->SetHeight(1);
	me->m_plot->SetPosition(0, 0);
	me->m_plot->SetPlotRange(0, 0, 360, 256);
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

void CD3DpictureView::SetImage(vavImage* img, ID3D11ShaderResourceView* tex)
{
	m_vavImage = img;
	m_hsvImage = new vavImage;
	*m_hsvImage = m_vavImage->Clone();
	m_hsvImage->ConvertToHSV();
	m_D3DApp.SetScale(m_Scale);
	m_D3DApp.SetTexture(tex);
	m_D3DApp.BuildPoint();
	m_D3DApp.DrawScene();
}

void CD3DpictureView::SetPictureSize(int w, int h)
{
	m_PicW = w;
	m_PicH = h;
	m_D3DApp.SetPictureSize(w, h);
}

ID3D11Device* CD3DpictureView::GetDevice()
{
	return m_D3DApp.GetDevice();
}

ID3D11DeviceContext* CD3DpictureView::GetDeviceContext()
{
	return m_D3DApp.GetDeviceContext();
}

void CD3DpictureView::SetLineRadius(float r)
{
	m_LineRadius = r;
	m_D3DApp.SetLineRadius(m_LineRadius);
	UpdateImageFeature();
}

void CD3DpictureView::SetMouseType(D3DApp_Picture::Shape s)
{
	m_D3DApp.SetMouseType(s);
	UpdateImageFeature();
}

void CD3DpictureView::UpdateImageFeature()
{
	D3DXVECTOR3 color;
	color.x = 1;
	color.y = 0;
	color.z = 0.5;
	double realx = (m_MouseMove.x - m_LookCenter.x) / m_Scale - m_LookCenter.x *
				   0.5;
	double realy = (m_PicH * m_Scale - m_D3DApp.Height() + m_MouseMove.y
					- m_LookCenter.y) / m_Scale - m_LookCenter.y * 0.5;
	LockDraw();
	switch (m_D3DApp.GetMouseType())
	{
	case D3DApp_Picture::CIRCLE_LINE:
		m_TimerCallback->m_data[0] = m_vavImage->GetRingLight(realx, realy,
									 m_LineRadius, 360);
		m_TimerCallback->m_data[1] = m_vavImage->GetRingR(realx, realy, m_LineRadius,
									 360);
		m_TimerCallback->m_data[2] = m_vavImage->GetRingG(realx, realy, m_LineRadius,
									 360);
		m_TimerCallback->m_data[3] = m_vavImage->GetRingB(realx, realy, m_LineRadius,
									 360);
		break;
	case D3DApp_Picture::VERTICAL_LINE:
		m_TimerCallback->m_data[0] = m_vavImage->GetVerticalLight(realx, realy,
									 m_LineRadius, 360);
		m_TimerCallback->m_data[1] = m_vavImage->GetVerticalR(realx, realy,
									 m_LineRadius, 360);
		m_TimerCallback->m_data[2] = m_vavImage->GetVerticalG(realx, realy,
									 m_LineRadius, 360);
		m_TimerCallback->m_data[3] = m_vavImage->GetVerticalB(realx, realy,
									 m_LineRadius, 360);
		break;
	case D3DApp_Picture::HORIZONTAL_LINE:
		m_TimerCallback->m_data[0] = m_vavImage->GetHorizontalLight(realx, realy,
									 m_LineRadius, 360);
		m_TimerCallback->m_data[1] = m_vavImage->GetHorizontalR(realx, realy,
									 m_LineRadius, 360);
		m_TimerCallback->m_data[2] = m_vavImage->GetHorizontalG(realx, realy,
									 m_LineRadius, 360);
		m_TimerCallback->m_data[3] = m_vavImage->GetHorizontalB(realx, realy,
									 m_LineRadius, 360);
		break;
	case D3DApp_Picture::NONE_LINE:
	{
		double_vector data;
		data.push_back(0);
		m_TimerCallback->m_data[0] = data;
		m_TimerCallback->m_data[1] = data;
		m_TimerCallback->m_data[2] = data;
		m_TimerCallback->m_data[3] = data;
	}
	break;
	}
	UnlockDraw();
	m_D3DApp.SetMousePoint(realx, realy, m_LineRadius * 2, color);
	m_D3DApp.BuildPoint();
	m_D3DApp.DrawScene();
// 	printf("px: %3.1f py: %3.1f Center.x %3.1f Center3.y %3.1f\n",
// 		realx, realy, m_LookCenter.x, m_LookCenter.y);
}

void CD3DpictureView::LockDraw()
{
	m_TimerCallback->Lock();
}

void CD3DpictureView::UnlockDraw()
{
	m_TimerCallback->Unlock();
}
