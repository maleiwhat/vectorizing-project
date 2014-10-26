#pragma once
#include <dwrite.h>
#include <d3dCompiler.h>
#include <d3dx11effect.h>
#include <d3dx10math.h>

#include "LineDef.h"
#include "DX11\Vertex.h"
#include "TriangulationBase.h"

class D3DApp;
class RegionDiffusion
{
public:
    RegionDiffusion(D3DApp* app);
    ~RegionDiffusion(void);
    // init part
	void OnResizeRelease();
    void OnResize(int w, int h);
    void BuildShaderFX();
    void BuildPoint();
    void ReadD3DSetting();
    // set render part
    void SetDiffusionRegions(int n);
	void AddColorTriangles(int r, const Triangles& ts);
    void AddDiffusionLinesRegion(int r, const Lines& lines, const Vector3s2d& colors);
    void AddDiffusionLinesRegion(int r, const Lines& lines, const Color2Side& colors);
	void SetRegions(Triangles2d& regions);
    // darw diffusion curve
    void AddDiffusionLines(const Lines& lines, const Vector3s2d& colors);
    void AddDiffusionLines(const Lines& lines, const Color2Side& colors);
    // darw line
    void AddLines(const Lines& lines);

    void InterSetSize(float w, float h);
    void InterSetLookCenter(float x, float y);
    void InterSetScale(float s);
    // render part
    void DrawScene();
	void InterDraw();
	
	Triangles2d	m_Region;
    bool    m_Init;
    int     m_PicW;
    int     m_PicH;
    int     m_ClientWidth;
    int     m_ClientHeight;
	float   m_Scale;
	float   m_LookCenterX;
	float   m_LookCenterY;
	float	m_Transparency_LineSkeleton;
	float	m_Transparency_SelectPatch;

    ID3D11Device*       m_pd3dDevice;
    IDXGISwapChain*     m_SwapChain;
    ID3D11Texture2D*    m_DepthStencilBuffer;
    ID3D11Texture2D*    m_DrawTextureDepthStencilBuffer;
    ID3D11Texture2D*    m_DrawTexture;
    ID3D11RenderTargetView* m_RenderTargetView;
    ID3D11RenderTargetView* m_distDirTextureTV;
    ID3D11ShaderResourceView* m_distDirTextureRV;
    ID3D11DepthStencilView* m_DepthStencilView;
    ID3D11DepthStencilView* m_DrawTextureDepthStencilView;
    ID3D11DeviceContext*    m_DeviceContext;
	ID3D11Texture2D*		m_BackBuffer;
	ID3D11BlendState*   m_pBlendState_BLEND;

    ID3D11DepthStencilState* m_pDepthStencil_ZWriteON;

    D3DApp*  m_D3DApp;
    
    ID3D11Buffer*                   m_SkeletonLines_Buffer;
    ID3DX11Effect*                  m_SkeletonLines_Effect;
    ID3DX11EffectTechnique*         m_SkeletonLines_PTech;
    ID3D11InputLayout*              m_SkeletonLines_PLayout;
    ID3DX11EffectScalarVariable*    m_SkeletonLines_Width;
    ID3DX11EffectScalarVariable*    m_SkeletonLines_Height;
    ID3DX11EffectScalarVariable*    m_SkeletonLines_Scale;
    ID3DX11EffectScalarVariable*    m_SkeletonLines_CenterX;
    ID3DX11EffectScalarVariable*    m_SkeletonLines_CenterY;
    ID3DX11EffectScalarVariable*    m_SkeletonLines_Alpha;
    SkeletonLineVertexes        m_SkeletonLinesVertices;
    // diffusion curve part
    D3D11_VIEWPORT m_vp;
    float m_fWidth;
    float m_fHeight;
    float m_polySize;
    int m_diffTex;
    int m_diffSteps;
    int m_cNum;
    int m_cSegNum;
    
    D3DXVECTOR2 m_pan;
    ID3D11InputLayout*      m_pCurveVertexLayout;
    ID3D11Buffer*           m_pCurveVertexBuffer;
    ID3D11InputLayout*      m_pCurveVertex2Layout;
    ID3D11Buffer*           m_pCurveVertex2Buffer;
	ID3D11Buffer*           m_pCurveVertex3Buffer;
    CURVE_Vertexes          m_CurveVertexes;
	CURVE_Vertexes2d		m_RegionCurveVertexes;
	ints					m_RegionOffset;
    VSO_Vertexes			m_RegionTrangles;

    // two textures used interleavedly for diffusion
    ID3D11Texture2D*    m_diffuseTexture[2];
    // two textures used interleavedly for diffusion (blurr texture)
    ID3D11Texture2D*    m_diffdistDirTexture;
    ID3D11Texture2D*    m_Texture;
    ID3D11Texture2D*    m_pDepthStencil;         // for z culling
    // texture that keeps the color on the other side of a curve
    ID3D11Texture2D*    m_otherTexture;
    
    ID3D11ShaderResourceView* m_diffuseTextureRV[2];
    ID3D11RenderTargetView*   m_diffuseTextureTV[2];
    ID3D11ShaderResourceView* m_diffdistDirTextureRV;
    ID3D11RenderTargetView*   m_diffdistDirTextureTV;
    ID3D11ShaderResourceView* m_TextureRV;
    ID3D11RenderTargetView*   m_TextureTV;
    ID3D11DepthStencilView*   m_pDepthStencilView;
    ID3D11ShaderResourceView* m_otherTextureRV;
    ID3D11RenderTargetView*   m_otherTextureTV;
    
    ID3DX11EffectTechnique* m_pDrawVectorsTechnique;
    ID3DX11EffectTechnique* m_pDiffuseTechnique;
    ID3DX11EffectTechnique* m_pLineAntiAliasTechnique;
    ID3DX11EffectTechnique* m_pDisplayImage;
	ID3DX11EffectTechnique* m_pDisplayImage2;
    
    ID3DX11EffectShaderResourceVariable* m_pInTex[3];
    ID3DX11EffectShaderResourceVariable* m_pDiffTex;
    ID3DX11EffectScalarVariable* m_pDiffX;
    ID3DX11EffectScalarVariable* m_blurOn;
    ID3DX11EffectScalarVariable* m_pDiffY;
    ID3DX11EffectScalarVariable* m_pScale;
    ID3DX11EffectVectorVariable* m_pPan;
    ID3DX11EffectScalarVariable* m_pPolySize;
	ID3DX11EffectScalarVariable* m_pAlpha;
    
    D3D11_BUFFER_DESC   m_vbd;
};

