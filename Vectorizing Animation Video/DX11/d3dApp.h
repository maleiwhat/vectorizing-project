#pragma once
#include <string>
#include <dwrite.h>
#include <d3dCompiler.h>
#include <d3dx11effect.h>
#include <opencv2/opencv.hpp>
#include "Timer.h"
#include "DXUTUI.h"
#include "Vertex.h"
#include "TriangulationBase.h"
#include "vavImage.h"
#include "LineDef.h"
#include "RegionDiffusion.h"

class D3DApp
{
public:
    RegionDiffusion m_RegionDiffusion;
    D3DApp();
    ~D3DApp();
    
    HINSTANCE   getAppInst();
    HWND        getMainWnd();
    
    void initApp(HWND hWnd, int w, int h);
    void OnResize(int w = 0, int h = 0); // reset projection/etc
    void OnDrawToBimapResize();
    void SetPictureSize(int w, int h);
    void SetLookCenter(float x, float y);
    void SetScale(float s);
    void SetScaleTemporary(float s);
    void SetScaleRecovery();
    void DrawScene();
    cv::Mat DrawSceneToCvMat(D3DXCOLOR backcolor = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f),
                             bool drawDiffusion = false);
    void LoadBlend();
    void BuildShaderFX();
    void BuildPoint();
    void AddTriangles(const ColorTriangles& tris, const vavImage& vimage);
    void AddColorTriangles(const ColorTriangles& tris);
    void AddTrianglesLine(const ColorTriangles& tris);
    void AddPatchTriangles(const ColorTriangles& tris);
    void AddBigPoint(float x, float y, D3DXVECTOR3 color);
    void AddLines(const Lines& lines, const double_vector2d& linewidths,
                  const Vector3s2d& colors);
    void AddLines(const Lines& lines, const double_vector2d& linewidths);
    void AddLinesWidth(const Lines& lines, const Lines& linewidths);
    void AddLines(const Lines& lines, const Lines& linewidths);
    void AddLines(const Lines& lines);
#ifdef USE_CGAL
    void AddLines(const CgalLines& lines);
#endif // USE_CGAL
    void AddLines(const CvLine& lines);
    void AddLinesWidth(const Lines& lines, const Lines& linewidths, const Vector3s2d& colors);
    void AddLinesCover(const Lines& lines);
    void AddLinesLine(const Lines& lines, const double_vector2d& linewidths);
    void AddLineSegs(const LineSegs& lines);
    void AddDiffusionLine(const Lines& lines, const Color2Side& colors, int idx);
    void AddDiffusionLines(const Lines& lines);
    void AddDiffusionLines(const Lines& lines, const Vector3s2d& colors);
    void AddDiffusionLines(const Lines& lines, const Color2Side& colors);
    
	void AddLayer();

    void ClearPatchs();
    void ClearCovers();
    void ClearSkeletonLines();
    void SetTransparency_Triangle(float t);
    void SetTransparency_SelectPatch(float t);
    void SetTransparency_TriangleLine(float t);
    void SetTransparency_Line(float t);
    void SetTransparency_LineSkeleton(float t);
    void SetTransparency_Picture(float t);
    
    void ClearTriangles();
    int  GetWidth();
    int  GetHeight();
    void SetTexture(ID3D11ShaderResourceView* tex);
    
    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();
    void InterSetRenderTransparencyOutput1();
    void InterSetRenderTransparencyOutput2();
    void InterSetRenderTransparencyOutput3();
	void InterSetRenderTransparencyOutput4();
    void InterSetRenderTransparencyDefault();
protected:
    void initDirect3D();
    void InterDraw(bool drawDiffusion = true);
    void InterSetLookCenter(float x, float y);
    void InterSetScale(float s);
    void InterSetSize(float w, float h);
protected:

    float m_Transparency_Triangle;
    float m_Transparency_SelectPatch;
    float m_Transparency_TriangleLine;
    float m_Transparency_Lines;
    float m_Transparency_LineSkeleton;
    float m_Transparency_Picture;
    
	ints	m_LayerTriangle;
	ints	m_LayerLines;


    int     m_PicW;
    int     m_PicH;
    float   m_Scale;
    float   m_ScaleSave;
    float   m_LookCenterX;
    float   m_LookCenterY;
    HINSTANCE   m_hAppInst;
    HWND        m_hMainWnd;
    bool        m_AppPaused;
    bool        m_Minimized;
    bool        m_Maximized;
    bool        m_Resizing;
    bool        m_Init;
    GameTimer   m_Timer;
    std::wstring    m_FrameStats;
    ID3D11Texture2D* m_BackBuffer;
    
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
    
    ID3D11BlendState*   m_pBlendState_ADD;
    ID3D11BlendState*   m_pBlendState_BLEND;
    ID3D11DepthStencilState* m_pDepthStencil_ZWriteON;
    ID3D11DepthStencilState* m_pDepthStencil_ZWriteOFF;
    
    ID3D11Buffer*                   m_Backup_Buffer;
    ID3D11Buffer*                   m_Pics_Buffer;
    ID3DX11Effect*                  m_Pics_Effect;
    ID3DX11EffectTechnique*         m_Pics_PTech;
    ID3D11InputLayout*              m_Pics_PLayout;
    ID3DX11EffectScalarVariable*    m_Pics_Width;
    ID3DX11EffectScalarVariable*    m_Pics_Height;
    ID3DX11EffectScalarVariable*    m_Pics_CenterX;
    ID3DX11EffectScalarVariable*    m_Pics_CenterY;
    ID3DX11EffectScalarVariable*    m_Pics_Scale;
    ID3DX11EffectScalarVariable*    m_TransparencySV_Picture;
    ID3DX11EffectShaderResourceVariable*  m_Pics_PMap;
    ID3D11ShaderResourceView*   m_Pics_Texture;
    PictureVertices         m_PicsVertices;
    
    ID3D11Buffer*                   m_Triangle_Buffer;
    ID3DX11Effect*                  m_Triangle_Effect;
    ID3DX11EffectTechnique*         m_Triangle_PTech;
    ID3D11InputLayout*              m_Triangle_PLayout;
    ID3DX11EffectScalarVariable*    m_Triangle_Width;
    ID3DX11EffectScalarVariable*    m_Triangle_Height;
    ID3DX11EffectScalarVariable*    m_Triangle_CenterX;
    ID3DX11EffectScalarVariable*    m_Triangle_CenterY;
    ID3DX11EffectScalarVariable*    m_Triangle_Scale;
    ID3DX11EffectScalarVariable*    m_Triangle_Alpha;
    TriangleVertices        m_TriangleVertices;
    
    ID3D11Buffer*                   m_TriangleLine_Buffer;
    ID3DX11Effect*                  m_TriangleLine_Effect;
    ID3DX11EffectTechnique*         m_TriangleLine_PTech;
    ID3D11InputLayout*              m_TriangleLine_PLayout;
    ID3DX11EffectScalarVariable*    m_TriangleLine_Width;
    ID3DX11EffectScalarVariable*    m_TriangleLine_Height;
    ID3DX11EffectScalarVariable*    m_TriangleLine_CenterX;
    ID3DX11EffectScalarVariable*    m_TriangleLine_CenterY;
    ID3DX11EffectScalarVariable*    m_TriangleLine_Scale;
    ID3DX11EffectScalarVariable*    m_TriangleLine_Alpha;
    TriangleVertices        m_TriangleLineVertices;
    
    ID3D11Buffer*                   m_Patch_Buffer;
    ID3DX11Effect*                  m_Patch_Effect;
    ID3DX11EffectTechnique*         m_Patch_PTech;
    ID3D11InputLayout*              m_Patch_PLayout;
    ID3DX11EffectScalarVariable*    m_Patch_Width;
    ID3DX11EffectScalarVariable*    m_Patch_Height;
    ID3DX11EffectScalarVariable*    m_Patch_CenterX;
    ID3DX11EffectScalarVariable*    m_Patch_CenterY;
    ID3DX11EffectScalarVariable*    m_Patch_Scale;
    ID3DX11EffectScalarVariable*    m_SelectPatch_Alpha;
    TriangleVertices        m_PatchVertices;
    
    ID3D11Buffer*                   m_Points_Buffer;
    ID3DX11Effect*                  m_Points_Effect;
    ID3DX11EffectTechnique*         m_Points_PTech;
    ID3D11InputLayout*              m_Points_PLayout;
    ID3DX11EffectScalarVariable*    m_Points_Width;
    ID3DX11EffectScalarVariable*    m_Points_Height;
    ID3DX11EffectScalarVariable*    m_Points_Scale;
    ID3DX11EffectScalarVariable*    m_Points_CenterX;
    ID3DX11EffectScalarVariable*    m_Points_CenterY;
    ID3DX11EffectScalarVariable*    m_Points_Alpha;
    PointVertices           m_PointsVertices;
    
    ID3D11Buffer*                   m_Lines_Buffer;
    ID3DX11Effect*                  m_Lines_Effect;
    ID3DX11EffectTechnique*         m_Lines_PTech;
    ID3D11InputLayout*              m_Lines_PLayout;
    ID3DX11EffectScalarVariable*    m_Lines_Width;
    ID3DX11EffectScalarVariable*    m_Lines_Height;
    ID3DX11EffectScalarVariable*    m_Lines_Scale;
    ID3DX11EffectScalarVariable*    m_Lines_CenterX;
    ID3DX11EffectScalarVariable*    m_Lines_CenterY;
    ID3DX11EffectScalarVariable*    m_Lines_Alpha;
    LineVertices            m_LinesVertices;
    
    ID3D11Buffer*                   m_Lines2w_Buffer;
    ID3DX11Effect*                  m_Lines2w_Effect;
    ID3DX11EffectTechnique*         m_Lines2w_PTech;
    ID3D11InputLayout*              m_Lines2w_PLayout;
    ID3DX11EffectScalarVariable*    m_Lines2w_Width;
    ID3DX11EffectScalarVariable*    m_Lines2w_Height;
    ID3DX11EffectScalarVariable*    m_Lines2w_Scale;
    ID3DX11EffectScalarVariable*    m_Lines2w_PeripheryAlpha;
    ID3DX11EffectScalarVariable*    m_Lines2w_CenterX;
    ID3DX11EffectScalarVariable*    m_Lines2w_CenterY;
    ID3DX11EffectScalarVariable*    m_Lines2w_CenterAlpha;
    Line2wVertices            m_Lines2wVertices;
    
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
    SkeletonLineVertexes        m_SkeletonLinesVerticesCover;
    
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
    CURVE_Vertexes          m_CurveVertexes;
    
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
    
    ID3DX11EffectShaderResourceVariable* m_pInTex[3];
    ID3DX11EffectShaderResourceVariable* m_pDiffTex;
    ID3DX11EffectScalarVariable* m_pDiffX;
    ID3DX11EffectScalarVariable* m_blurOn;
    ID3DX11EffectScalarVariable* m_pDiffY;
    ID3DX11EffectScalarVariable* m_pScale;
    ID3DX11EffectVectorVariable* m_pPan;
    ID3DX11EffectScalarVariable* m_pPolySize;
	ID3DX11EffectScalarVariable* m_pAlpha;
    // diffusion curve part end
    
    D3D11_BUFFER_DESC   m_vbd;
    DXUTUI*             m_DXUT_UI;
    // Derived class should set these in derived constructor to customize starting values.
    D3D_DRIVER_TYPE     m_d3dDriverType;
    D3DXCOLOR           m_ClearColor;
    int     m_ClientWidth;
    int     m_ClientHeight;
    friend RegionDiffusion;
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
    operator const D3D11_BLEND_DESC& () const
    {
        return *this;
    }
};

