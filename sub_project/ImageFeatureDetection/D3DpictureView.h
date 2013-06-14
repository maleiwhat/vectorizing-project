#pragma once

#include "DX11\D3DApp_Picture.h"
#include "math\Vector2.h"

#include <vtkSphereSource.h>
#include <vtkMath.h>
#include <vtkDoubleArray.h>
#include <vtkFieldData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkXYPlotActor.h>
#include <vtkCubeSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkTesting.h>
#include "vavImage.h"


class vtkTimerCallback : public vtkCommand
{
public:
	double_vector2d m_data;
	vtkXYPlotActor_Sptr m_plot;
	vtkRenderWindow_Sptr m_renderWindow;
	vtkRenderWindowInteractor_Sptr m_interactor;
	static vtkTimerCallback* New()
	{
		vtkTimerCallback* cb = new vtkTimerCallback;
		cb->m_data.push_back(double_vector());
		cb->m_data.push_back(double_vector());
		cb->m_data.push_back(double_vector());
		cb->m_data.push_back(double_vector());
		cb->m_data[0].push_back(0);
		cb->m_data[1].push_back(0);
		cb->m_data[2].push_back(0);
		cb->m_data[3].push_back(0);
		return cb;
	}

	virtual void Execute(vtkObject* vtkNotUsed(caller), unsigned long eventId,
		void* vtkNotUsed(callData))
	{
		m_plot->RemoveAllInputs();

		for (unsigned int i = 0 ; i < m_data.size() ; i++)
		{
			vtkSmartPointer<vtkDoubleArray> array_s =
				vtkSmartPointer<vtkDoubleArray>::New();
			vtkSmartPointer<vtkFieldData> field =
				vtkSmartPointer<vtkFieldData>::New();
			vtkSmartPointer<vtkDataObject> data =
				vtkSmartPointer<vtkDataObject>::New();

			for (int b = 0; b < m_data[i].size(); b++)
			{
				array_s->InsertValue(b, m_data[i][b]);
			}

			field->AddArray(array_s);
			data->SetFieldData(field);
			m_plot->AddDataObjectInput(data);
		}

		m_renderWindow->Render();
	}


};


// CD3DpictureView 檢視

class CD3DpictureView : public CView
{
	DECLARE_DYNCREATE(CD3DpictureView)
private:
	CPoint      m_MouseDown,
	            m_MouseUp,
	            m_MouseMove;
	Vector2     m_LookCenter;
	Vector2     m_LookDown;
	int     m_PicW;
	int     m_PicH;
	float	m_Scale;

	vtkTimerCallback_Sptr m_vtkTimerCallback;
	vtkXYPlotActor_Sptr m_plot;
	vtkRenderWindow_Sptr m_renderWindow;
	vtkRenderWindowInteractor_Sptr m_interactor;

	HWND        m_hWndDX11;
	D3DApp_Picture  m_D3DApp;
	bool        m_MButtonDown;
	bool        m_LButtonDown;
	vavImage*	m_vavImage;
	vavImage*	m_hsvImage;
public:
	
	void Init();
	void SetImage(vavImage* img, ID3D11ShaderResourceView* tex)
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
	void SetPictureSize(int w, int h)
	{
		m_PicW = w;
		m_PicH = h;
		m_D3DApp.SetPictureSize(w, h);
	}
	ID3D11Device* GetDevice()
	{
		return m_D3DApp.GetDevice();
	}
	ID3D11DeviceContext* GetDeviceContext()
	{
		return m_D3DApp.GetDeviceContext();
	}


	static unsigned __stdcall MyThreadFunc(LPVOID lpParam);
protected:
	void InitDx11(HWND hWnd);
private:
	void Update(int x, int y);
	void PreBuild(int x, int y);
protected:
	CD3DpictureView();           // 動態建立所使用的保護建構函式
	virtual ~CD3DpictureView();

public:
	virtual void OnDraw(CDC* pDC);      // 覆寫以描繪此檢視
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseLeave();
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
extern CD3DpictureView* g_NewPictureView;

