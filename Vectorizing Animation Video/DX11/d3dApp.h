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
#include "Line.h"

class D3DApp
{
public:
	D3DApp();
	~D3DApp();

	HINSTANCE   getAppInst();
	HWND        getMainWnd();

	void initApp(HWND hWnd, int w, int h);
	void OnResize(int w, int h);   // reset projection/etc
	void SetPictureSize(int w, int h);
	void SetLookCenter(float x, float y);
	void SetScale(float s);
	void DrawScene();
	void LoadBlend();
	void BuildShaderFX();
	void BuildPoint();
	void AddTriangles(const Triangles& tris, const vavImage& vimage);
	void AddColorTriangles(const Triangles& tris);
	void AddTrianglesLine(const Triangles& tris);
	void AddPatchTriangles(const Triangles& tris);
	void AddBigPoint(float x, float y, D3DXVECTOR3 color);
	void AddLines(const Lines& lines, const double_vector2d& linewidths,
	              const Vector3s2d& colors);
	void AddLines(const Lines& lines, const double_vector2d& linewidths);
	void AddLinesLine(const Lines& lines, const double_vector2d& linewidths);
	void AddLines(const Lines& lines);
	void AddLineSegs(const LineSegs& lines);
	void AddLines(const Points2d& lines);
	void ClearPatchs();
	void SetPatchTransparency(float t);
	void SetSelectPatchTransparency(float t);
	void SetTriangleLineTransparency(float t);
	void SetLineTransparency(float t);
	void SetLineSkeletonTransparency(float t);
	void SetPictureTransparency(float t);
	void ClearTriangles();
	int  Width() { return m_ClientWidth;}
	int  Height() {return m_ClientHeight;};
	void SetTexture(ID3D11ShaderResourceView* tex);
	
	ID3D11Device* GetDevice()
	{
		return m_d3dDevice;
	}
	ID3D11DeviceContext* GetDeviceContext()
	{
		return m_DeviceContext;
	}
protected:
	void initDirect3D();
	void InterDraw();
	void InterSetLookCenter(float x, float y);
	void InterSetScale(float s);
	void InterSetSize(float w, float h);
protected:
	int     m_PicW;
	int     m_PicH;
	float	m_Scale;
	float	m_LookCenterX;
	float	m_LookCenterY;
	HINSTANCE   m_hAppInst;
	HWND        m_hMainWnd;
	bool        m_AppPaused;
	bool        m_Minimized;
	bool        m_Maximized;
	bool        m_Resizing;
	GameTimer   m_Timer;
	std::wstring    m_FrameStats;
	ID3D11Texture2D* m_BackBuffer;

	ID3D11Device*       m_d3dDevice;
	IDXGISwapChain*     m_SwapChain;
	ID3D11Texture2D*    m_DepthStencilBuffer;
	ID3D11Texture2D*    m_DrawTextureDepthStencilBuffer;
	ID3D11Texture2D*    m_DrawTexture;
	ID3D11DepthStencilView* m_DepthStencilView2;
	ID3D11RenderTargetView* m_RenderTargetView;
	ID3D11RenderTargetView* m_distDirTextureTV;
	ID3D11ShaderResourceView* m_distDirTextureRV;
	ID3D11DepthStencilView* m_DepthStencilView;
	ID3D11DepthStencilView* m_DrawTextureDepthStencilView;
	ID3D11DeviceContext*    m_DeviceContext;

	ID3D11BlendState*   m_pBlendState_ADD;
	ID3D11BlendState*   m_pBlendState_BLEND;
	ID3D11DepthStencilState* m_pDepthStencil_ZWriteON;
	ID3D11DepthStencilState* m_pDepthStencil_ZWriteOFF;

	ID3D11Buffer*           m_Backup_Buffer;
	ID3D11Buffer*           m_Pics_Buffer;
	ID3DX11Effect*          m_Pics_Effect;
	ID3DX11EffectTechnique*     m_Pics_PTech;
	ID3D11InputLayout*      m_Pics_PLayout;
	ID3DX11EffectScalarVariable*    m_Pics_Width;
	ID3DX11EffectScalarVariable*    m_Pics_Height;
	ID3DX11EffectScalarVariable*    m_Pics_CenterX;
	ID3DX11EffectScalarVariable*    m_Pics_CenterY;
	ID3DX11EffectScalarVariable*    m_Pics_Scale;
	ID3DX11EffectScalarVariable*    m_Pics_Transparency;
	ID3DX11EffectShaderResourceVariable*  m_Pics_PMap;
	ID3D11ShaderResourceView*   m_Pics_Texture;
	PictureVertices         m_PicsVertices;

	ID3D11Buffer*           m_Triangle_Buffer;
	ID3DX11Effect*          m_Triangle_Effect;
	ID3DX11EffectTechnique*     m_Triangle_PTech;
	ID3D11InputLayout*      m_Triangle_PLayout;
	ID3DX11EffectScalarVariable*    m_Triangle_Width;
	ID3DX11EffectScalarVariable*    m_Triangle_Height;
	ID3DX11EffectScalarVariable*    m_Triangle_CenterX;
	ID3DX11EffectScalarVariable*    m_Triangle_CenterY;
	ID3DX11EffectScalarVariable*    m_Triangle_Scale;
	ID3DX11EffectScalarVariable*    m_Triangle_Transparency;
	TriangleVertices        m_TriangleVertices;

	ID3D11Buffer*           m_TriangleLine_Buffer;
	ID3DX11Effect*          m_TriangleLine_Effect;
	ID3DX11EffectTechnique*     m_TriangleLine_PTech;
	ID3D11InputLayout*      m_TriangleLine_PLayout;
	ID3DX11EffectScalarVariable*    m_TriangleLine_Width;
	ID3DX11EffectScalarVariable*    m_TriangleLine_Height;
	ID3DX11EffectScalarVariable*    m_TriangleLine_CenterX;
	ID3DX11EffectScalarVariable*    m_TriangleLine_CenterY;
	ID3DX11EffectScalarVariable*    m_TriangleLine_Scale;
	ID3DX11EffectScalarVariable*    m_TriangleLine_Transparency;
	TriangleVertices        m_TriangleLineVertices;

	ID3D11Buffer*           m_Patch_Buffer;
	ID3DX11Effect*          m_Patch_Effect;
	ID3DX11EffectTechnique*     m_Patch_PTech;
	ID3D11InputLayout*      m_Patch_PLayout;
	ID3DX11EffectScalarVariable*    m_Patch_Width;
	ID3DX11EffectScalarVariable*    m_Patch_Height;
	ID3DX11EffectScalarVariable*    m_Patch_CenterX;
	ID3DX11EffectScalarVariable*    m_Patch_CenterY;
	ID3DX11EffectScalarVariable*    m_Patch_Scale;
	ID3DX11EffectScalarVariable*    m_Patch_Transparency;
	TriangleVertices        m_PatchVertices;

	ID3D11Buffer*           m_Points_Buffer;
	ID3DX11Effect*          m_Points_Effect;
	ID3DX11EffectTechnique*     m_Points_PTech;
	ID3D11InputLayout*      m_Points_PLayout;
	ID3DX11EffectScalarVariable*    m_Points_Width;
	ID3DX11EffectScalarVariable*    m_Points_Height;
	ID3DX11EffectScalarVariable*    m_Points_Scale;
	ID3DX11EffectScalarVariable*    m_Points_CenterX;
	ID3DX11EffectScalarVariable*    m_Points_CenterY;
	ID3DX11EffectScalarVariable*    m_Points_Transparency;
	PointVertices           m_PointsVertices;

	ID3D11Buffer*           m_Lines_Buffer;
	ID3DX11Effect*          m_Lines_Effect;
	ID3DX11EffectTechnique*     m_Lines_PTech;
	ID3D11InputLayout*      m_Lines_PLayout;
	ID3DX11EffectScalarVariable*    m_Lines_Width;
	ID3DX11EffectScalarVariable*    m_Lines_Height;
	ID3DX11EffectScalarVariable*    m_Lines_Scale;
	ID3DX11EffectScalarVariable*    m_Lines_CenterX;
	ID3DX11EffectScalarVariable*    m_Lines_CenterY;
	ID3DX11EffectScalarVariable*    m_Lines_Transparency;
	LineVertices            m_LinesVertices;

	ID3D11Buffer*           m_SkeletonLines_Buffer;
	ID3DX11Effect*          m_SkeletonLines_Effect;
	ID3DX11EffectTechnique*     m_SkeletonLines_PTech;
	ID3D11InputLayout*      m_SkeletonLines_PLayout;
	ID3DX11EffectScalarVariable*    m_SkeletonLines_Width;
	ID3DX11EffectScalarVariable*    m_SkeletonLines_Height;
	ID3DX11EffectScalarVariable*    m_SkeletonLines_Scale;
	ID3DX11EffectScalarVariable*    m_SkeletonLines_CenterX;
	ID3DX11EffectScalarVariable*    m_SkeletonLines_CenterY;
	ID3DX11EffectScalarVariable*    m_SkeletonLines_Transparency;
	SkeletonLineVertexes        m_SkeletonLinesVertices;

	D3D11_BUFFER_DESC       m_vbd;
	DXUTUI*             m_DXUT_UI;
	// Derived class should set these in derived constructor to customize starting values.
	std::wstring    m_MainWndCaption;
	D3D_DRIVER_TYPE m_d3dDriverType;
	D3DXCOLOR   m_ClearColor;
	int     m_ClientWidth;
	int     m_ClientHeight;
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

