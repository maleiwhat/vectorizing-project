#pragma once
#include <string>
#include <dwrite.h>
#include <d3dCompiler.h>
#include <d3dx11effect.h>
#include "Timer.h"
#include "DXUTUI.h"
#include "Vertex.h"
#include "TriangulationBase.h"
#include "vavImage.h"

class D3DApp
{
public:
	D3DApp();
	~D3DApp();

	HINSTANCE	getAppInst();
	HWND		getMainWnd();

	void initApp( HWND hWnd, int w, int h );
	void OnResize( int w, int h ); // reset projection/etc
	void SetPictureSize( int w, int h );
	void SetLookCenter( float x, float y );
	void SetScale( float s );
	void DrawScene();
	void LoadBlend();
	void BuildShaderFX();
	void BuildPoint();
	void AddTriangles( Triangles& tris, float h, const vavImage& vimage );
	void AddColorTriangles( Triangles& tris, float h );
	void AddTrianglesLine( Triangles& tris, float h);
	void AddPatchTriangles( Triangles& tris, float h );
	void AddBigPoint(float x, float y, float h, D3DXVECTOR3 color);
	void ClearPatchs();
	void SetPatchTransparency(float t);
	void SetSelectPatchTransparency(float t);
	void SetLineTransparency(float t);
	void SetPictureTransparency(float t);
	void ClearTriangles();
	int  Width() { return mClientWidth;}
	int  Height() {return mClientHeight;};
	void SetTexture( ID3D11ShaderResourceView* tex );
	ID3D11Device* GetDevice()
	{
		return m_d3dDevice;
	}
protected:
	void initDirect3D();
	
protected:
	int		m_PicW;
	int		m_PicH;
	HINSTANCE	m_hAppInst;
	HWND		m_hMainWnd;
	bool		m_AppPaused;
	bool		m_Minimized;
	bool		m_Maximized;
	bool		m_Resizing;
	GameTimer	m_Timer;
	std::wstring	m_FrameStats;
	ID3D11Device*		m_d3dDevice;
	IDXGISwapChain*		m_SwapChain;
	ID3D11Texture2D*	m_DepthStencilBuffer;
	ID3D11DepthStencilView* m_DepthStencilView2;
	ID3D11RenderTargetView* m_RenderTargetView;
	ID3D11DepthStencilView* m_DepthStencilView;
	ID3D11DeviceContext*	m_DeviceContext;

	ID3D11BlendState*	m_pBlendState_ADD;
	ID3D11BlendState*	m_pBlendState_BLEND;
	ID3D11DepthStencilState* m_pDepthStencil_ZWriteON;
	ID3D11DepthStencilState* m_pDepthStencil_ZWriteOFF;

	ID3DX11Effect*			m_Effect_Pics;
	ID3DX11EffectTechnique*		m_PTech_Pics;
	ID3D11InputLayout*		m_PLayout_Pics;
	ID3DX11EffectScalarVariable*	m_Pics_Width;
	ID3DX11EffectScalarVariable*	m_Pics_Height;
	ID3DX11EffectScalarVariable*	m_Pics_CenterX;
	ID3DX11EffectScalarVariable*	m_Pics_CenterY;
	ID3DX11EffectScalarVariable*	m_Pics_Scale;
	ID3DX11EffectScalarVariable*	m_Pics_Transparency;
	ID3DX11EffectShaderResourceVariable*  m_PMap_Pics;
	ID3D11ShaderResourceView*	m_Pics_Texture;
	ID3D11Buffer*			m_Buffer_Pics;
	PictureVertices			m_PictureVertices;

	ID3DX11Effect*			m_Effect_Triangle;
	ID3DX11EffectTechnique*		m_PTech_Triangle;
	ID3D11InputLayout*		m_PLayout_Triangle;
	ID3DX11EffectScalarVariable*	m_Triangle_Width;
	ID3DX11EffectScalarVariable*	m_Triangle_Height;
	ID3DX11EffectScalarVariable*	m_Triangle_CenterX;
	ID3DX11EffectScalarVariable*	m_Triangle_CenterY;
	ID3DX11EffectScalarVariable*	m_Triangle_Scale;
	ID3DX11EffectScalarVariable*	m_Triangle_Transparency;
	ID3D11Buffer*			m_Buffer_Triangle;
	TriangleVertices		m_TriangleVertices;

	ID3DX11Effect*			m_Effect_TriangleLine;
	ID3DX11EffectTechnique*		m_PTech_TriangleLine;
	ID3D11InputLayout*		m_PLayout_TriangleLine;
	ID3DX11EffectScalarVariable*	m_TriangleLine_Width;
	ID3DX11EffectScalarVariable*	m_TriangleLine_Height;
	ID3DX11EffectScalarVariable*	m_TriangleLine_CenterX;
	ID3DX11EffectScalarVariable*	m_TriangleLine_CenterY;
	ID3DX11EffectScalarVariable*	m_TriangleLine_Scale;
	ID3DX11EffectScalarVariable*	m_TriangleLine_Transparency;
	ID3D11Buffer*			m_Buffer_TriangleLine;
	TriangleVertices		m_TriangleLineVertices;

	ID3DX11Effect*			m_Effect_Patch;
	ID3DX11EffectTechnique*		m_PTech_Patch;
	ID3D11InputLayout*		m_PLayout_Patch;
	ID3DX11EffectScalarVariable*	m_Patch_Width;
	ID3DX11EffectScalarVariable*	m_Patch_Height;
	ID3DX11EffectScalarVariable*	m_Patch_CenterX;
	ID3DX11EffectScalarVariable*	m_Patch_CenterY;
	ID3DX11EffectScalarVariable*	m_Patch_Scale;
	ID3DX11EffectScalarVariable*	m_Patch_Transparency;
	ID3D11Buffer*			m_Buffer_Patch;
	TriangleVertices		m_PatchVertices;

	ID3D11Buffer*			m_Buffer_Points;
	ID3DX11Effect*			m_Effect_Points;
	ID3DX11EffectTechnique*		m_PTech_Points;
	ID3D11InputLayout*		m_PLayout_Points;
	ID3DX11EffectScalarVariable*	m_Points_Width;
	ID3DX11EffectScalarVariable*	m_Points_Height;
	ID3DX11EffectScalarVariable*	m_Points_Scale;
	ID3DX11EffectScalarVariable*	m_Points_CenterX;
	ID3DX11EffectScalarVariable*	m_Points_CenterY;
	ID3DX11EffectScalarVariable*	m_Points_Transparency;
	PointVertices			m_PointsVertices;

	D3D11_BUFFER_DESC		m_vbd;
	DXUTUI*				m_DXUT_UI;
	// Derived class should set these in derived constructor to customize starting values.
	std::wstring	m_MainWndCaption;
	D3D_DRIVER_TYPE	m_d3dDriverType;
	D3DXCOLOR	m_ClearColor;
	int		mClientWidth;
	int		mClientHeight;
};

struct CD3D11_BLEND_DESCX : public D3D11_BLEND_DESC
{
	CD3D11_BLEND_DESCX() {}
	explicit CD3D11_BLEND_DESCX( const D3D11_BLEND_DESC& o ) : D3D11_BLEND_DESC( o ) {}
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
	        UINT8 RenderTargetWriteMask0 )
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
