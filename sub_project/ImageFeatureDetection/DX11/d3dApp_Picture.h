#pragma once
#include <string>
#include <dwrite.h>
#include <d3dCompiler.h>
#include <d3dx11effect.h>
#include "Timer.h"
#include "Vertex.h"
#include "../Line.h"

class D3DApp_Picture
{
public:
	int     m_PicW;
	int     m_PicH;
	enum Shape
	{
		CIRCLE_LINE = 1,
		VERTICAL_LINE,
		HORIZONTAL_LINE,
		NONE_LINE
	};
private:
public:
	D3DApp_Picture();
	~D3DApp_Picture();

	HINSTANCE   getAppInst();
	HWND        getMainWnd();

	void initApp(HWND hWnd, int w, int h);
	void SetPictureSize(int w, int h);
	void OnResize(int w, int h);   // reset projection/etc
	void DrawScene();
	void LoadBlend();
	void BuildShaderFX();
	void BuildPoint();
	void InterSetSize(float w, float h);
	void InterSetLookCenter(float x, float y);
	void InterSetScale(float s);
	void SetTexture(ID3D11ShaderResourceView* tex);
	void SetScale(float s);
	void SetLookCenter(float x, float y);
	void AddBigPoint(float x, float y, float radius, D3DXVECTOR3 color);
	void SetMousePoint(float x, float y, float radius, D3DXVECTOR3 color);
	void AddLine(const Line& lines, D3DXVECTOR3 color);
	void SetMouseType(Shape s);
	Shape GetMouseType();
	int  Width() { return m_ClientWidth;}
	int  Height() {return m_ClientHeight;}
	void ClearTriangles();
	void InterDraw();
	void SetLineRadius(float r);
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
protected:
	void initDirect3D();
protected:
	Shape		m_MouseType;
	float		m_LineRadius;
	PointVertex m_MousePoint;
	SkeletonLineVertex m_MouseVertical;
	SkeletonLineVertex m_MouseHorizontal;
	HINSTANCE   m_hAppInst;
	HWND        m_hMainWnd;
	bool        m_AppPaused;
	bool        m_Minimized;
	bool        m_Maximized;
	bool        m_Resizing;
	GameTimer   m_Timer;
	int     m_ClientWidth;
	int     m_ClientHeight;
	float   m_LookCenterX;
	float   m_LookCenterY;
	float   m_Scale;
	std::wstring    m_FrameStats;
	ID3D11Device*   m_d3dDevice;
	ID3D11DeviceContext*    m_DeviceContext;
	IDXGISwapChain* m_SwapChain;
	ID3D11Texture2D*    m_DepthStencilBuffer;
	ID3D11DepthStencilView* m_DepthStencilView2;
	ID3D11RenderTargetView* m_RenderTargetView;
	ID3D11DepthStencilView* m_DepthStencilView;

	ID3D11BlendState*   m_pBlendState_ADD;
	ID3D11BlendState*   m_pBlendState_BLEND;
	ID3D11DepthStencilState* m_pDepthStencil_ZWriteON;
	ID3D11DepthStencilState* m_pDepthStencil_ZWriteOFF;

	ID3D11Buffer*					m_Backup_Buffer;
	ID3D11Buffer*					m_Pics_Buffer;
	ID3DX11Effect*					m_Pics_Effect;
	ID3DX11EffectTechnique*			m_Pics_PTech;
	ID3D11InputLayout*				m_Pics_PLayout;
	ID3DX11EffectScalarVariable*    m_Pics_Width;
	ID3DX11EffectScalarVariable*    m_Pics_Height;
	ID3DX11EffectScalarVariable*    m_Pics_CenterX;
	ID3DX11EffectScalarVariable*    m_Pics_CenterY;
	ID3DX11EffectScalarVariable*    m_Pics_Scale;
	ID3DX11EffectScalarVariable*    m_Pics_Transparency;
	ID3DX11EffectShaderResourceVariable*  m_Pics_PMap;
	ID3D11ShaderResourceView*		m_Pics_Texture;
	PictureVertices					m_PicsVertices;

	ID3D11Buffer*					m_Points_Buffer;
	ID3DX11Effect*					m_Points_Effect;
	ID3DX11EffectTechnique*			m_Points_PTech;
	ID3D11InputLayout*				m_Points_PLayout;
	ID3DX11EffectScalarVariable*    m_Points_Width;
	ID3DX11EffectScalarVariable*    m_Points_Height;
	ID3DX11EffectScalarVariable*    m_Points_Scale;
	ID3DX11EffectScalarVariable*    m_Points_CenterX;
	ID3DX11EffectScalarVariable*    m_Points_CenterY;
	ID3DX11EffectScalarVariable*    m_Points_Transparency;
	PointVertices					m_PointsVertices;

	ID3D11Buffer*					m_CircleLine_Buffer;
	ID3DX11Effect*					m_CircleLine_Effect;
	ID3DX11EffectTechnique*			m_CircleLine_PTech;
	ID3D11InputLayout*				m_CircleLine_PLayout;
	ID3DX11EffectScalarVariable*    m_CircleLine_Width;
	ID3DX11EffectScalarVariable*    m_CircleLine_Height;
	ID3DX11EffectScalarVariable*    m_CircleLine_Scale;
	ID3DX11EffectScalarVariable*    m_CircleLine_CenterX;
	ID3DX11EffectScalarVariable*    m_CircleLine_CenterY;
	ID3DX11EffectScalarVariable*    m_CircleLine_Transparency;
	PointVertices					m_CircleLineVertices;

	ID3D11Buffer*					m_SkeletonLines_Buffer;
	ID3DX11Effect*					m_SkeletonLines_Effect;
	ID3DX11EffectTechnique*			m_SkeletonLines_PTech;
	ID3D11InputLayout*				m_SkeletonLines_PLayout;
	ID3DX11EffectScalarVariable*    m_SkeletonLines_Width;
	ID3DX11EffectScalarVariable*    m_SkeletonLines_Height;
	ID3DX11EffectScalarVariable*    m_SkeletonLines_Scale;
	ID3DX11EffectScalarVariable*    m_SkeletonLines_CenterX;
	ID3DX11EffectScalarVariable*    m_SkeletonLines_CenterY;
	ID3DX11EffectScalarVariable*    m_SkeletonLines_Transparency;
	SkeletonLineVertexes			m_SkeletonLinesVertices;

	D3D11_BUFFER_DESC       m_vbd;
	// Derived class should set these in derived constructor to customize starting values.
	std::wstring m_MainWndCaption;
	D3D_DRIVER_TYPE m_d3dDriverType;
	D3DXCOLOR m_ClearColor;
	D3D_FEATURE_LEVEL  m_FeatureLevelsSupported;
};


struct CD3D11_BLEND_DESCX : public D3D11_BLEND_DESC
{
	CD3D11_BLEND_DESCX() {}
	explicit CD3D11_BLEND_DESCX(const D3D11_BLEND_DESC& o) : D3D11_BLEND_DESC(o) {}
	explicit CD3D11_BLEND_DESCX(
	    BOOL AlphaToCoverageEnable,
	    BOOL IndependentBlendEnable,
	    BOOL BlendEnable0,
	    D3D11_BLEND SrcBlend0,
	    D3D11_BLEND DestBlend0,
	    D3D11_BLEND_OP BlendOp0,
	    D3D11_BLEND SrcBlendAlpha0,
	    D3D11_BLEND DestBlendAlpha0,
	    D3D11_BLEND_OP BlendOpAlpha0,
	    UINT8 RenderTargetWriteMask0)
	{
		this->AlphaToCoverageEnable = AlphaToCoverageEnable;
		this->IndependentBlendEnable = IndependentBlendEnable;
		this->RenderTarget[0].BlendEnable = BlendEnable0;
		this->RenderTarget[0].SrcBlend = SrcBlend0;
		this->RenderTarget[0].DestBlend = DestBlend0;
		this->RenderTarget[0].BlendOp = BlendOp0;
		this->RenderTarget[0].SrcBlendAlpha = SrcBlendAlpha0;
		this->RenderTarget[0].DestBlendAlpha = DestBlendAlpha0;
		this->RenderTarget[0].BlendOpAlpha = BlendOpAlpha0;
		this->RenderTarget[0].RenderTargetWriteMask = RenderTargetWriteMask0;
	}
	~CD3D11_BLEND_DESCX() {}
	operator const D3D11_BLEND_DESC& () const { return *this; }
};

