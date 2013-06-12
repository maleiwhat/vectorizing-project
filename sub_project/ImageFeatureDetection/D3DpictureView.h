#pragma once

#include "DX11\D3DApp_Picture.h"

// CD3DpictureView 檢視


class CD3DpictureView : public CView
{
	DECLARE_DYNCREATE( CD3DpictureView )
private:
	bool m_TrackMouse;
	bool m_LMouseHold;
	bool m_MMouseHold;
	bool m_CtrlPress;
	bool m_ShiftPress;
	bool m_KeyAPress;

	float m_Scale;
public:
	int		m_PictureID;
	HWND		m_hWndDX11;
	D3DApp_Picture	m_D3DApp;

	void Init();
	void SetTexture(ID3D11ShaderResourceView* tex)
	{
		m_D3DApp.SetScale(m_Scale);
		m_D3DApp.SetTexture(tex);
		m_D3DApp.BuildPoint();
		m_D3DApp.DrawScene();
	}
	void SetPictureSize(int w, int h)
	{
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
protected:
	void InitDx11( HWND hWnd );
private:
	void Update( int x, int y );
	void PreBuild( int x,int y);
protected:
	CD3DpictureView();           // 動態建立所使用的保護建構函式
	virtual ~CD3DpictureView();

public:
	virtual void OnDraw( CDC* pDC );    // 覆寫以描繪此檢視
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump( CDumpContext& dc ) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnMouseMove( UINT nFlags, CPoint point );
	afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	afx_msg void OnMouseLeave();
	afx_msg void OnMButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnMButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnPaint();
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
};
extern CD3DpictureView* g_NewPictureView;

